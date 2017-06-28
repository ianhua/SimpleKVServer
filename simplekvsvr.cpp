#include "simplekvsvr.h"

using namespace mmtraining;

/**
 * Simple Key-Value server
 * @author ianhua
 */

// Logger
std::ofstream logger;
/************************Test functions************************/
void TestIndex() {

    // MAX_CACHE_SIZE 64

    SimpleKVIndex index;
    SimpleKVRecord* item1 = new SimpleKVRecord();
    SimpleKVRecord* item2 = new SimpleKVRecord();
    SimpleKVRecord* item3 = new SimpleKVRecord();
    // Set item
    item1->valid = true;
    std::string key1("ian");
    std::string value1("ian is testing SimpleKVSvr!");
    item1->key = (char*)malloc(key1.length() + 1);
    strcpy(item1->key, key1.c_str());
    item1->value = (char*)malloc(value1.length() + 1);
    strcpy(item1->value, value1.c_str());
    item1->offset = 0;
    item1->length = value1.length();
    item1->prev = NULL;
    item1->next = NULL;

    item2->valid = true;
    std::string key2("boxer");
    std::string value2("boxer is testing SimpleKVSvr!");
    item2->key = (char*)malloc(key2.length() + 1);
    strcpy(item2->key, key2.c_str());
    item2->value = (char*)malloc(value2.length() + 1);
    strcpy(item2->value, value2.c_str());
    item2->offset = 0;
    item2->length = value2.length();
    item2->prev = NULL;
    item2->next = NULL;

    item3->valid = true;
    std::string key3("delphi");
    std::string value3("delphi is testing SimpleKVSvr!");
    item3->key = (char*)malloc(key3.length() + 1);
    strcpy(item3->key, key3.c_str());
    item3->value = (char*)malloc(value3.length() + 1);
    strcpy(item3->value, value3.c_str());
    item3->offset = 0;
    item3->length = value3.length();
    item3->prev = NULL;
    item3->next = NULL;

    index.Set(item1);
    index.PrintCache(); // ian

    index.Set(item2);
    index.PrintCache(); // boxer, ian

    index.Set(item3);
    index.PrintCache(); // delphi, boxer

    SimpleKVRecord* item;
    if (index.Get("ian", item) == 0) {
        if (item->value != NULL) {
            std::cout << "Get ian: " << std::string(item->value) << std::endl;
        } else {
            std::cout << "Get ian: ian is in file" << std::endl;
        }
    }

    // As if read from file
    item->value = (char*)malloc(item->length + 1);
    strcpy(item->value, value1.c_str());
    std::cout << "Read from file: " << std::string(item->value) << std::endl;

    index.PrintCache(); // ian, delphi

    index.Delete("ian");

    index.PrintCache(); // delphi

    if (index.Get("ian", item) == 1) {
        std::cout << "ian does not exist!" << std::endl;
    }

    // key: 2, mem_size: 31, hit: 0, miss: 1
    std::cout << index.Stats() << std::endl;

    free(item1->key);
    free(item1->value);
    free(item1);
    free(item2->key);
    free(item2->value);
    free(item2);
    free(item3->key);
    free(item3->value);
    free(item3);

}

void TestStore() {

    SimpleKVStore store;
    SimpleKVIndex index;

    store.Init("storefile");

    //store.Load(index);

    SimpleKVRecord* item1 = new SimpleKVRecord();
    SimpleKVRecord* item2 = new SimpleKVRecord();
    SimpleKVRecord* item3 = new SimpleKVRecord();
    // Set item
    item1->valid = true;
    std::string key1("ian");
    std::string value1("ian is testing SimpleKVSvr!");
    item1->key = (char*)malloc(key1.length() + 1);
    strcpy(item1->key, key1.c_str());
    item1->value = (char*)malloc(value1.length() + 1);
    strcpy(item1->value, value1.c_str());
    item1->offset = 0;
    item1->length = value1.length();
    item1->prev = NULL;
    item1->next = NULL;

    item2->valid = true;
    std::string key2("boxer");
    std::string value2("boxer is testing SimpleKVSvr!");
    item2->key = (char*)malloc(key2.length() + 1);
    strcpy(item2->key, key2.c_str());
    item2->value = (char*)malloc(value2.length() + 1);
    strcpy(item2->value, value2.c_str());
    item2->offset = 0;
    item2->length = value2.length();
    item2->prev = NULL;
    item2->next = NULL;

    item3->valid = true;
    std::string key3("boxer");
    std::string value3("boxer is not testing SimpleKVSvr!");
    item3->key = (char*)malloc(key3.length() + 1);
    strcpy(item3->key, key3.c_str());
    item3->value = (char*)malloc(value3.length() + 1);
    strcpy(item3->value, value3.c_str());
    item3->offset = 0;
    item3->length = value3.length();
    item3->prev = NULL;
    item3->next = NULL;

    // Test Write
    store.Write(*item1);

    store.Write(*item2);

    store.Write(*item3);

    store.WriteDelete("ian");

    // Test Load
    //store.Load(index);

    // key_count: 1
    //std::cout << index.Stats() << std::endl;

    //SimpleKVRecord* item;
    /*if (index.Get("ian", item) == 1) {
        std::cout << "ian does not exist!" << std::endl;
    }

    if (index.Get("boxer", item) == 0) {
        std::cout << "Get boxer: " << std::string(item->value) << std::endl;
    }*/


    // Test Read
    //free(item1->value);
    //item1->value = NULL;
    //store.Read(*item1);
    //std::cout << "Read from file: " << std::string(item1->value) << std::endl;

    store.Compact();

    // Test Stats
    std::cout << store.Stats() << std::endl;

    free(item1->key);
    free(item1->value);
    free(item1);
    free(item2->key);
    free(item2->value);
    free(item2);
    free(item3->key);
    free(item3->value);
    free(item3);
}

/************************Helper functions************************/
std::string& Trim(std::string& str) {
    const std::string delimters = " \f\n\r\t\v";
    str.erase(str.find_last_not_of(delimters) + 1);
    str.erase(0, str.find_first_not_of(delimters));
    return str;
}

/************************SimpleKVCache***************************/
SimpleKVCache::SimpleKVCache() : mem_size(0), head(NULL), tail(NULL) {}

SimpleKVCache::~SimpleKVCache() {}

void SimpleKVCache::AddItem(SimpleKVRecord* item) {
    // Update mem_size
    mem_size += item->length + 1;

    // Add item to LRU linked list head
    SimpleKVRecord* first = head;
    if (first == NULL) {
        head = item;
        tail = item;
        item->prev = NULL;
        item->next = NULL;
    } else {
        head = item;
        item->prev = NULL;
        item->next = first;
        first->prev = item;
    }

    // Evict if exceed cache size
    SimpleKVRecord* evictItem;
    while (mem_size > MAX_CACHE_SIZE) {
        evictItem = tail;
        RemoveItem(evictItem);
        free(evictItem->value);
        evictItem->value = NULL;
    }
}

void SimpleKVCache::RemoveItem(SimpleKVRecord* item) {
    // Update mem_size
    mem_size -= item->length + 1;

    // Remove item from LRU linked list
    SimpleKVRecord *prev_item, *next_item;
    if (item->prev == NULL && item->next == NULL) {
        // Only one item in linked list
        head = NULL;
        tail = NULL;
    } else if (item->prev == NULL && item->next != NULL) {
        // The first item in linked list
        next_item = item->next;
        next_item->prev = NULL;
        head = next_item;
        item->next = NULL;
    } else if (item->prev != NULL && item->next == NULL) {
        // The last item in linked list
        prev_item = item->prev;
        prev_item->next = NULL;
        tail = prev_item;
        item->prev = NULL;
    } else {
        // In the middle of linked list
        prev_item = item->prev;
        next_item = item->next;
        prev_item->next = next_item;
        next_item->prev = prev_item;
        item->prev = NULL;
        item->next = NULL;
    }
}

/************************SimpleKVIndex***************************/

SimpleKVIndex::SimpleKVIndex() : key_count(0), hit_count(0), miss_count(0) {
    // Initialize rwlock
    if (pthread_rwlock_init(&rwlock, NULL) != 0) {
        logger << "rwlock initialize error!" << std::endl;
    }

    // Initialize mutex
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        logger << "mutex initialize error!" << std::endl;
    }
}

SimpleKVIndex::~SimpleKVIndex() {
    // Destroy rwlock
    if (pthread_rwlock_destroy(&rwlock) != 0) {
        logger << "rwlock destroy error!" << std::endl;
    }

    // Destroy mutex
    if (pthread_mutex_destroy(&mutex) != 0) {
        logger << "mutex destroy error!" << std::endl;
    }
}

int SimpleKVIndex::Get(const char* key, SimpleKVRecord *&item) {
    // Lock rdlock
    if (pthread_rwlock_rdlock(&rwlock) != 0) {
        logger << "Index rwlock lock error!" << std::endl;
        return -1;
    }

    std::string key_str(key);
    std::tr1::unordered_map<std::string, SimpleKVRecord*>::const_iterator iter = 
    hashmap.find(key_str);

    if (iter != hashmap.end()) {
        // Key exists
        item = iter->second;

        // Lock mutex
        if (pthread_mutex_lock(&mutex) != 0) {
            logger << "cache lock error!" << std::endl;
            return -1;
        }

        if (item->value != NULL) {
            // Value is in memory, cache hit
            hit_count++;
            cache.RemoveItem(item);
            cache.AddItem(item);
        } else {
            // Value is in file, cache miss
            miss_count++;
            cache.AddItem(item);
        }

        // Unlock mutex
        if (pthread_mutex_unlock(&mutex) != 0) {
            logger << "cache unlock error!" << std::endl;
            return -1;
        }

        // Unlock rdlock
        if (pthread_rwlock_unlock(&rwlock) != 0) {
            logger << "Index rwlock unlock error!" << std::endl;
            return -1;
        }

    } else {
        // Key does not exist
        // Unlock rdlock
        if (pthread_rwlock_unlock(&rwlock) != 0) {
            logger << "Index rwlock unlock error!" << std::endl;
            return -1;
        }
        return 1;
    }

    return 0;
}

int SimpleKVIndex::Set(SimpleKVRecord* item) {
    // Lock rdlock
    if (pthread_rwlock_wrlock(&rwlock) != 0) {
        logger << "Index rwlock lock error!" << std::endl;
        return -1;
    }

    std::string key_str(item->key);
    std::tr1::unordered_map<std::string, SimpleKVRecord*>::const_iterator iter = 
    hashmap.find(key_str);

    SimpleKVRecord* oldItem;

    if (iter != hashmap.end()) {
        // Key exists
        oldItem = iter->second; 

        if (pthread_mutex_lock(&mutex) != 0) {
            logger << "cache lock error!" << std::endl;
            return -1;
        }

        if (oldItem->value != NULL) {
            // Value is in cache
            cache.RemoveItem(oldItem);
            free(oldItem->key);
            free(oldItem->value);
            free(oldItem);
            cache.AddItem(item);
        } else {
            // Value is in file
            free(oldItem->key);
            free(oldItem);
            cache.AddItem(item);
        }

        // Unlock mutex
        if (pthread_mutex_unlock(&mutex) != 0) {
            logger << "cache unlock error!" << std::endl;
            return -1;
        }

        hashmap[key_str] = item;

        // Unlock wrlock
        if (pthread_rwlock_unlock(&rwlock) != 0) {
            logger << "Index rwlock unlock error!" << std::endl;
            return -1;
        }

    } else {
        // Key does not exist
        // Lock mutex
        if (pthread_mutex_lock(&mutex) != 0) {
            logger << "cache lock error!" << std::endl;
            return -1;
        }

        cache.AddItem(item);

        // Unlock mutex
        if (pthread_mutex_unlock(&mutex) != 0) {
            logger << "cache unlock error!" << std::endl;
            return -1;
        }

        // Update key_count and hashmap
        key_count++;
        hashmap[key_str] = item;

        // Unlock wrlock
        if (pthread_rwlock_unlock(&rwlock) != 0) {
            logger << "Index rwlock unlock error!" << std::endl;
            return -1;
        }

    }
    return 0;
}

int SimpleKVIndex::Delete(const char* key) {
    // Lock rdlock
    if (pthread_rwlock_wrlock(&rwlock) != 0) {
        logger << "Index rwlock lock error!" << std::endl;
        return -1;
    }

    std::string key_str(key);
    std::tr1::unordered_map<std::string, SimpleKVRecord*>::const_iterator iter = 
    hashmap.find(key_str);

    SimpleKVRecord* oldItem;

    if (iter != hashmap.end()) {
        // Key exists
        oldItem = iter->second;

        // Lock mutex
        if (pthread_mutex_lock(&mutex) != 0) {
            logger << "cache lock error!" << std::endl;
            return -1;
        }

        if (oldItem->value != NULL) {
            // Value is in memory
            cache.RemoveItem(oldItem);
            free(oldItem->key);
            free(oldItem->value);
            free(oldItem);
        } else {
            // Value is in file
            free(oldItem->key);
            free(oldItem);
        }

        // Unlock mutex
        if (pthread_mutex_unlock(&mutex) != 0) {
            logger << "cache unlock error!" << std::endl;
            return -1;
        }

        // Delete hashmap item
        key_count--;
        hashmap.erase(key_str);

        // Unlock wrlock
        if (pthread_rwlock_unlock(&rwlock) != 0) {
            logger << "Index rwlock unlock error!" << std::endl;
            return -1;
        }

    } else {
        // Key does not exist
        // Unlock wrlock
        if (pthread_rwlock_unlock(&rwlock) != 0) {
            logger << "Index rwlock unlock error!" << std::endl;
            return -1;
        }
        return 1;
    }

    return 0;
}

void SimpleKVIndex::PrintCache() {
    SimpleKVRecord* iter = cache.head;
    while (iter != NULL) {
        std::cout << std::string(iter->value) << " ->" << std::endl;
        iter = iter->next;
    }
    std::cout << std::endl;
}

std::string SimpleKVIndex::Stats() {
    std::ostringstream response;
    response << "+count: <" << key_count << ">, mem: <" << cache.mem_size << 
    ">, hits: <" << hit_count << ">, misses: <" << miss_count << ">, ";
    return response.str();
}

/************************SimpleKVStore***************************/

SimpleKVStore::SimpleKVStore() : fd(0) {
    // Initialize rwlock
    if (pthread_rwlock_init(&rwlock, NULL) != 0) {
        logger << "rwlock initialize error!" << std::endl;
    }
}

SimpleKVStore::~SimpleKVStore() {
    // Destroy rwlock
    if (pthread_rwlock_destroy(&rwlock) != 0) {
        logger << "rwlock destroy error!" << std::endl;
    }
    close(fd);
}

int SimpleKVStore::Init(const char* file) {
    strcpy(fileName, "/home/ianhua/homework/");
    strcat(fileName, file);

    if ((fd = open(fileName, O_RDWR | O_APPEND | O_CREAT, 00644)) == -1) {
        return -1;
    }
    return 0;
}

int SimpleKVStore::Load(SimpleKVIndex& index) {
    // Clear hashmap
   index.hashmap.clear();

    // Start from file head
    if (lseek(fd, 0, SEEK_SET) == -1) {
        return -1;
    }
    
    // Keep read until EOF
    while (true) {

        int res;
        SimpleKVRecord* item = new SimpleKVRecord();

        // valid
        bool valid;
        if ((res = read(fd, &valid, sizeof(bool))) == -1) {
            return -1;
        } else if (res == 0) {
            // EOF
            break;
        }
        item->valid = valid;

        // key
        int key_length;
        if (read(fd, &key_length, sizeof(int)) == -1) {
            return -1;
        }
        item->key = (char*)malloc(key_length + 1);
        if (read(fd, item->key, key_length + 1) == -1) {
            return -1;
        }

        // Erase key in hashmap if deleted
        if (valid == false) {
            index.Delete(item->key);
            continue;
        }

        // length
        int value_length;
        if (read(fd, &value_length, sizeof(int)) == -1) {
            return -1;
        }
        item->length = value_length;

        // offset
        item->offset = lseek(fd, 0, SEEK_CUR);

        // value
        item->value = (char*)malloc(value_length + 1);
        if (read(fd, item->value, value_length + 1) == -1) {
            return -1;
        }

        // Set new item
        index.Set(item);
    }

    return 0;
}

int SimpleKVStore::Read(SimpleKVRecord& record) {
    // Lock rdlock
    if (pthread_rwlock_rdlock(&rwlock) != 0) {
        logger << "Index rwlock lock error!" << std::endl;
        return -1;
    }

    // Set offset to value
    int pos = record.offset;
    if (lseek(fd, pos, SEEK_SET) == -1) {
        return -1;
    }

    // Read from file to heap
    record.value = (char*)malloc(record.length + 1);
    read(fd, record.value, record.length + 1);

    // Unlock rdlock
    if (pthread_rwlock_unlock(&rwlock) != 0) {
        logger << "Index rwlock unlock error!" << std::endl;
        return -1;
    }

    return 0;
}

int SimpleKVStore::Write(SimpleKVRecord& record) {

    // Lock wrlock
    if (pthread_rwlock_wrlock(&rwlock) != 0) {
        logger << "Index rwlock lock error!" << std::endl;
        return -1;
    }

    // Set offset to EOF
    if (lseek(fd, 0, SEEK_END) == -1) {
        return -1;
    }

    bool valid = record.valid;
    int key_length = strlen(record.key);
    int value_length = record.length;
    
    // Write five fields separately
    if (write(fd, &valid, sizeof(bool)) == -1) {
        return -1;
    }
    if (write(fd, &key_length, sizeof(int)) == -1) {
        return -1;
    }
    if (write(fd, record.key, key_length + 1) == -1) {
        return -1;
    }
    if (write(fd, &value_length, sizeof(int)) == -1) {
        return -1;
    }
    // Set offset of record
    off_t pos;
    if ((pos = lseek(fd, 0, SEEK_CUR)) == -1) {
        return -1;
    }
    record.offset = pos;

    if (write(fd, record.value, value_length + 1) == -1) {
        return -1;
    }

    // Unlock wrlock
    if (pthread_rwlock_unlock(&rwlock) != 0) {
        logger << "Index rwlock unlock error!" << std::endl;
        return -1;
    }

    return 0;
}

int SimpleKVStore::WriteDelete(const char* key) {
    // Lock wrlock
    if (pthread_rwlock_wrlock(&rwlock) != 0) {
        logger << "Index rwlock lock error!" << std::endl;
        return -1;
    }

    // Set offset to EOF
    if (lseek(fd, 0, SEEK_END) == -1) {
        return -1;
    }

    bool valid = false;
    int key_length = strlen(key);

    // Write three fields separately
    if (write(fd, &valid, sizeof(bool)) == -1) {
        return -1;
    }
    if (write(fd, &key_length, sizeof(int)) == -1) {
        return -1;
    }
    if (write(fd, key, key_length + 1) == -1) {
        return -1;
    }

    // Unlock wrlock
    if (pthread_rwlock_unlock(&rwlock) != 0) {
        logger << "Index rwlock unlock error!" << std::endl;
        return -1;
    }
    return 0;
}

int SimpleKVStore::Compact() {
    // Load old file to index
    SimpleKVIndex index;
    if (Load(index) == -1) {
        return -1;
    }

    // Close current fd
    close(fd);

    // Open new fd to overwrite the file
    fd = open(fileName, O_RDWR | O_TRUNC, 00644);

    // Traverse hashmap to write file
    std::tr1::unordered_map<std::string, SimpleKVRecord*>::iterator iter = 
    index.hashmap.begin();
    while (iter != index.hashmap.end()) {
        Write(*(iter->second));
        iter++;
    }

    return 0;
}

std::string SimpleKVStore::Stats() {
    // Lock rdlock
    if (pthread_rwlock_rdlock(&rwlock) != 0) {
        logger << "Index rwlock lock error!" << std::endl;
    }

    struct stat buf;
    int file_size;

    if (fstat(fd, &buf) == -1) {
        file_size = -1;
    } else {
        file_size = buf.st_size;
    }

    // Unlock rdlock
    if (pthread_rwlock_unlock(&rwlock) != 0) {
        logger << "Index rwlock unlock error!" << std::endl;
    }

    std::ostringstream response;
    response << "file: <" << file_size << ">\n";
    return response.str();
}

/************************SimpleKVWork***************************/
SimpleKVWork::SimpleKVWork(ClientSocket* clientSocket, 
    SimpleKVIndex& simpleKVIndex, SimpleKVStore& simpleKVStore) : 
    serverConnector(clientSocket), index(simpleKVIndex), store(simpleKVStore) 
    {}

SimpleKVWork::~SimpleKVWork() {
    delete serverConnector;
}

bool SimpleKVWork::NeedDelete() const {
    return true;
}

int SimpleKVWork::DoWork() {
    std::string request, response = "Quit!\n";

    // Keep doing work for the client until quit
    while (true) {

        // Read request from client
        if (serverConnector->ReadLine(request) == -1) {
            response = "+Read request error!\n";
            serverConnector->WriteLine(response);
            continue;
        }

        // Trim request
        if (!request.empty()) {
            request = Trim(request);
        }

        if (request.empty()) {
            response = "+Empty string!\n";
            serverConnector->WriteLine(response);
            continue;
        }

        // Resolve request
        if (request[0] != '-') {
            response = "+Request format error!\n";
            serverConnector->WriteLine(response);
            continue;
        }

        int cmdpos = request.find_first_of(" ", 1);
        int start, end;
        int len = request.length();
        std::string cmd, key, value;

        if (cmdpos != std::string::npos) {
            cmd = request.substr(1, cmdpos - 1);
            if (cmd == "get") {

                // Resolve key
                start = request.find_first_of("<", cmdpos + 1);
                if (start == std::string::npos) {
                    response = "+Unsupported command!\n";
                    serverConnector->WriteLine(response);
                    continue;
                }
                end = request.find_first_of(">", start + 1);
                if (end == std::string::npos) {
                    response = "+Unsupported command!\n";
                    serverConnector->WriteLine(response);
                    continue;
                }
                key = request.substr(start + 1, end - start - 1);
                if (end + 1 != len) {
                    response = "+Unsupported command!\n";
                    serverConnector->WriteLine(response);
                    continue;
                }

                // Get item from hashmap and update cache
                SimpleKVRecord* item;
                int res;
                if ((res = index.Get(key.c_str(), item)) == 0) {
                    // Key exists
                    if (item->value != NULL) {
                        // Value is in memory
                        value = std::string(item->value);
                    } else {
                        // Value is in file
                        if (store.Read(*item) != 0) {
                            response = "Read item error!\n";
                            serverConnector->WriteLine(response);
                            continue;
                        }
                        value = std::string(item->value);
                    }
                    response = "+<" + value + ">\n";
                } else if (res == 1) {
                    // key does not exist
                    response = "+<" + key + "> does not exist!\n";
                } else {
                    response = "+get fail!\n";
                }

            } else if (cmd == "set") {

                // Resolve key and value
                start = request.find_first_of("<", cmdpos + 1);
                if (start == std::string::npos) {
                    response = "+Unsupported command!\n";
                    serverConnector->WriteLine(response);
                    continue;
                }
                end = request.find_first_of(">", start + 1);
                if (end == std::string::npos) {
                    response = "+Unsupported command!\n";
                    serverConnector->WriteLine(response);
                    continue;
                }
                key = request.substr(start + 1, end - start - 1);
                start = request.find_first_of("<", end + 1);
                if (start == std::string::npos) {
                    response = "+Unsupported command!\n";
                    serverConnector->WriteLine(response);
                    continue;
                }
                end = request.find_first_of(">", start + 1);
                if (end == std::string::npos) {
                    response = "+Unsupported command!\n";
                    serverConnector->WriteLine(response);
                    continue;
                }
                value = request.substr(start + 1, end - start - 1);
                if (end + 1 != len) {
                    response = "+Unsupported command!\n";
                    serverConnector->WriteLine(response);
                    continue;
                }

                // Set item members
                SimpleKVRecord* item = new SimpleKVRecord();
                item->valid = true;
                item->length = value.length();
                item->prev = NULL;
                item->next = NULL;
                // Allocate item key
                item->key = (char*)malloc(key.length() + 1);
                strcpy(item->key, key.c_str());
                // Allocate item value
                item->value = (char*)malloc(value.length() + 1);
                strcpy(item->value, value.c_str());

                // Append to store first and get offset
                if (store.Write(*item) != 0) {
                    response = "+Write item error!\n";
                    serverConnector->WriteLine(response);
                    continue;
                }

                // Set item to hashmap and update cache
                if (index.Set(item) != 0) {
                    response = "+set fail!\n";
                } else {
                    response = "+OK\n";
                }

            } else if (cmd == "delete") {

                // Resolve key
                start = request.find_first_of("<", cmdpos + 1);
                if (start == std::string::npos) {
                    response = "+Unsupported command!\n";
                    serverConnector->WriteLine(response);
                    continue;
                }
                end = request.find_first_of(">", start + 1);
                if (end == std::string::npos) {
                    response = "+Unsupported command!\n";
                    serverConnector->WriteLine(response);
                    continue;
                }
                key = request.substr(start + 1, end - start - 1);
                if (end + 1 != len) {
                    response = "+Unsupported command!\n";
                    serverConnector->WriteLine(response);
                    continue;
                } 

                // Delete key
                int res;
                if ((res = index.Delete(key.c_str())) == 0) {
                    // Append delete record to store
                    if (store.WriteDelete(key.c_str()) != 0) {
                        response = "+Write item error!\n";
                        serverConnector->WriteLine(response);
                        continue;
                    }
                    response = "+OK\n";
                } else if (res == 1) {
                    response = "+<" + key + "> does not exist!\n";
                } else {
                    response = "+delete fail!\n";
                }

            } else {
                response = "+Unsupported command!\n";
            }
        } else {
            if (request.substr(1) == "stats") {

                // stats
                response = index.Stats() + store.Stats();

            } else if (request.substr(1) == "quit") {

                // quit
                response = "+OK\n";
                serverConnector->WriteLine(response);
                // End while loop
                break;

            } else {
                response = "+Unsupported command!\n";
            }
        }

        // Write response to client
        serverConnector->WriteLine(response);
    }

    return 0;
}

/************************SimpleKVServer***************************/
SimpleKVServer::SimpleKVServer() {}

SimpleKVServer::~SimpleKVServer() {}

int SimpleKVServer::Start(const char* ip, unsigned port, const char* file) {
    // Become daemon
    if (InitDaemon() != 0) {
        std::cout << "Become daemon error!" << std::endl;
        return -1;
    }

    // Open log file and overwrite it for every start
    logger.open("/home/ianhua/homework/simplekvsvrlog", 
        std::ofstream::out | std::ofstream::trunc);

    logger << "Server start!" << std::endl;

    // Listen port
    if (serverSocket.Listen(ip, port) != 0) {
        logger << "Listen error!" << std::endl;
        return -1;
    }

    logger << "Listening on " << ip << ":" << port << std::endl;
    
    // Initialize store
    if (store.Init(file) != 0) {
        logger << "Initialize store error!" << std::endl;
        return -1;
    }
    
    // Initialize index
    if (store.Load(index) != 0) {
        logger << "Initialize index error!" << std::endl;
        return -1;
    }
    
    // Start thread pool
    if (pool.Start(10) != 0) {
        logger << "Start thread pool error!" << std::endl;
        return -1;
    }
    
    // Let thread pool to handle works from clients
    while (true) {

        // Accept connection from client
        ClientSocket* serverConnector = serverSocket.Accept();
        if (serverConnector == NULL) {
            logger << "Accept error: " << std::endl;
            return -1;
        }
        
        // Add to work queue
        if (pool.AddWork(new SimpleKVWork(serverConnector, index, store)) == -1) {
            logger << "AddWork error!" << std::endl;
            return -1;
        }

    }

    store.Compact();

    pool.Shutdown();
    logger << "Server shutdown!" << std::endl;
    logger.close();

    return 0;
}

int SimpleKVServer::InitDaemon() {
    pid_t pid;
    int i, fd0, fd1, fd2;
    struct rlimit rl;
    struct sigaction sa;

    // Clear file mode creation mask
    umask(0);

    // Get maximum number of file descriptors
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        return -1;
    }

    // Fork child process
    if ((pid = fork()) < 0) {
        return -1;
    } else if (pid != 0) {
        exit(0);
    }

    // Become a session leader to lose controlling TTY
    setsid();

    // Ensure future opens won't allocate controlling TTYs
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        return -1;
    }
    if ((pid = fork()) < 0) {
        return -1;
    } else if (pid != 0) {
        exit(0);
    }

    // Change current working directory to the root
    if (chdir("/") < 0) {
        return -1;
    }

    // Close all open file descriptors
    if (rl.rlim_max == RLIM_INFINITY) {
        rl.rlim_max = 1024;
    }
    for (i = 0; i < rl.rlim_max; i++) {
        close(i);
    }
    
    // Attach file descriptors 0, 1 and 2 to /dev/null
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);
    
    return 0;
}

int main(int argc, char** argv) {
    /*if (argc < 4) {
        std::cout << "usage: " << argv[0] << " <ip> <port> <file-path>" << std::endl;
        return 1;
    }*/

    //return server.Start(argv[1], (unsigned short)atoi(argv[2]), argv[3]);

    //TestIndex();
    //TestStore();

    if (argc < 2) {
        std::cout << "usage: " << argv[0] << " <file-path>" << std::endl;
        return 1;
    }

    SimpleKVServer server;
    
    const char* ip = "127.0.0.1";

    unsigned short port = 7777;

    if (server.Start(ip, port, argv[1]) != 0) {
        logger << "Simple KV server start fail!" << std::endl;
        return -1;
    }

    return 0;
}
