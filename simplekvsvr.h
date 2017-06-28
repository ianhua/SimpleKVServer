
#ifndef MM_TRANING_SIMPLE_KV_H
#define MM_TRANING_SIMPLE_KV_H

#include "socket.h"
#include "threadpool.h"
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <errno.h>
#include <tr1/unordered_map>

using namespace mmtraining;

// Max cache size is 1MB
#define MAX_CACHE_SIZE 1048576

// For test
//#define MAX_CACHE_SIZE 64

/**
 * Simple Key-Value server header
 * @author ianhua
 */

/**
 * Test SimpleKVIndex
 */
void TestIndex();

/**
 * Test SimpleKVStore
 */
void TestStore();

/**
 * Helper functions
 */
std::string& Trim(std::string& str);

/**
 * Key-Value storage record
 */
struct SimpleKVRecord {

    /* Is valid */
    bool valid;

    /* Key */
    char* key;        
    /* NULL: not in memory, need to read file; NOT NULL: cached in memory */
    char* value;

    /* Offet of value in file */
    off_t offset; 
    /* Length of value */    
    size_t length;

    /* LRU linked list prev pointer */
    SimpleKVRecord* prev;
    /* LRU linked list next pointer */
    SimpleKVRecord* next;

};

/**
 * Simple KV cache
 * Cache with LRU policy implemented with linked list
 */
class SimpleKVCache {
public:

    /**
     * SimpleKVIndex is friend
     */
    friend class SimpleKVIndex;

    /**
     * Constructor
     */
    SimpleKVCache();

    /**
     * Destructor
     */
    ~SimpleKVCache();

    /**
     * Add item to LRU linked list head and update mem_size
     * @return 0 if success, -1 if fail
     */
    void AddItem(SimpleKVRecord* item);

    /**
     * Remove item from LRU linked list and update mem_size
     * @return 0 if success, -1 if fail
     */
    void RemoveItem(SimpleKVRecord* item);

private:

    int mem_size;

    // Head pointer for LRU linked list
    SimpleKVRecord* head;

    // Tail pointer for LRU linked list
    SimpleKVRecord* tail;
  
};

/**
 * Simple KV index
 * Maintain a hashmap and a cache for key-value pairs
 */
class SimpleKVIndex {
public:

    /**
     * SimpleKVStore is friend
     */
    friend class SimpleKVStore;

    /**
     * Constructor
     */
    SimpleKVIndex();

    /**
     * Destructor
     */
    ~SimpleKVIndex();

    /**
     * Get value and update LRU linked list
     * @return 0 if success, 1 if key does not exist, -1 if fail
     */
    int Get(const char* key, SimpleKVRecord *&item);

    /**
     * Set key-value and update LRU linked list
     * @return 0 if success, -1 if fail
     */
    int Set(SimpleKVRecord* item);

    /**
     * Delete key and update LRU linked list
     * @return 0 if success, 1 if key does not exist, -1 if fail
     */
    int Delete(const char* key);

    /**
     * Print values of LRU linked list for test
     */
    void PrintCache();

    /**
     * Print statistic string
     */
    std::string Stats();
    
private:

    SimpleKVCache cache;

    int key_count;

    int hit_count;

    int miss_count;

    // HashMap<Key, SimpleKVRecord>
    std::tr1::unordered_map<std::string, SimpleKVRecord*> hashmap;

    // Reader-writer lock for hashmap
    pthread_rwlock_t rwlock;

    // mutex for access LRU linked list
    pthread_mutex_t mutex;

};

/**
 * Simple KV store by append
 */
class SimpleKVStore {
public:

    /**
     * Constructor
     */
    SimpleKVStore();

    /**
     * Destructor
     */
    ~SimpleKVStore();

    /**
     * Initialize storage file
     * @return 0 if success, -1 if fail
     */
    int Init(const char* file);

    /**
     * Load storage file to index
     * @return 0 if success, -1 if fail
     */
    int Load(SimpleKVIndex& index);

    /**
     * Read record
     * @param record need offset and length, output value when finish
     * @return 0 if success, -1 if fail
     */
    int Read(SimpleKVRecord& record);

    /**
     * Write record
     * @param record need key, value and length, output offset when finish
     * @return 0 if success, -1 if fail
     */
    int Write(SimpleKVRecord& record);

    /**
     * Write delete record
     * @return 0 if success, -1 if fail
     */
    int WriteDelete(const char* key);

    /**
     * Remove deleted records, regenerate storage file
     * @return 0 if success, -1 if fail
     */
    int Compact();

    /**
     * Print statistic string
     */
    std::string Stats();
    
private:

    int fd;

    char* fileName;

    // Reader-writer lock
    pthread_rwlock_t rwlock;

};

/**
 * Simple KV business logic
 *
 * protocal
 * -: request; +: response
 *
 * 1) get
 * -get <key>\n
 * +<value>\n
 *
 * 2) set(override if exists, add if not)
 * -set <key> <value>\n
 * +OK\n
 *
 * 3) delete key-value
 * -delete <key>\n
 * +OK\n
 * 
 * 4) stats
 * -stats\n
 * +count: <key-count>, mem: <mem-size>, file: <file-size>,
 *  hits: <hit-count>, misses: <miss-count>\n
 *
 * 5) quit
 * -quit\n
 * +OK\n
 */
class SimpleKVWork : public Work {
public:

    /**
     * Constructor with ClientSocket, SimpleKVIndex and SimpleKVStore
     */
    SimpleKVWork(ClientSocket* clientSocket, SimpleKVIndex& simpleKVIndex, 
        SimpleKVStore& simpleKVStore);
    
    /**
     * Destructor
     */
    virtual ~SimpleKVWork();

    /**
     * Is need to be deleted after finishing work
     */
    virtual bool NeedDelete() const;

    /**
     * Business logic
     * @return 0 if success, -1 if fail
     */
    virtual int DoWork();

private:

    ClientSocket* serverConnector;

    SimpleKVIndex& index;

    SimpleKVStore& store;

};

/**
 * Simple KV server
 */
class SimpleKVServer {
public:

    /**
     * Constructor
     */
    SimpleKVServer();

    /**
     * Destructor
     */
    ~SimpleKVServer();

    /**
     * Start server
     * @return 0 if success, -1 if fail
     */
    int Start(const char* ip, unsigned port, const char* file);

private:

    /**
     * Become daemon
     * @return 0 if success, -1 if fail
     */
    int InitDaemon();

    SimpleKVIndex index;

    SimpleKVStore store;

    ServerSocket serverSocket;
    
    WorkerThreadPool pool;

};

#endif // MM_TRANNING_SIMPLE_KV_H
