
#ifndef MM_TRANING_THREAD_POOL_H
#define MM_TRANING_THREAD_POOL_H

#include <pthread.h>
#include <deque>
#include <vector>
#include "runnable.h"

/**
 * Thread pool header
 * @author ianhua
 */

namespace mmtraining {

/**
 * Thread
 */
class Thread : public Runnable {
public:
    /**
     * Constructor
     */
    Thread();
    
    /**
     * Constructor with Runnable
     */
    Thread(Runnable& t);

    /**
     * Destructor
     */
    virtual ~Thread();
    
    /**
     * Start thread
     * @return 0 if success, -1 if fail
     */
    int Start();
    
    /**
     * Get thread id
     */
    pthread_t GetId() const;

    /**
     * Run thread
     * @return 0 if success, -1 if fail
     */
    virtual int Run();
    
    /**
     * Is running
     */
    bool IsRunning() const;
    
    /**
     * Wait for this thread to finish and reap it
     * @return 0 if success, -1 if fail
     */
    int Join();

    /**
     * Exit thread
     */
    void Exit();
    
protected:

    /**
     * Run wrapper
     */
    static void* pthread_func(void* args);

    /**
     * Default Run function
     */
    virtual int DoRun();

    pthread_t tid;
    
    bool running;

    Runnable* target;

};

/**
 * Thread pool with constant number of threads
 */
class ThreadPool {
public:
    /**
     * Constructor
     */
    ThreadPool();

    /**
     * Destructor
     */
    virtual ~ThreadPool();

    /**
     * Start ThreadPool and do target function
     * @param threadCount thread count
     * @param target task function
     * @return 0 if success, -1 if fail
     */
    virtual int Start(int threadCount, Runnable& target);

    /**
     * Wait for all threads in the pool to finish and reap them
     * @return 0 if success, -1 if fail
     */
    int JoinAll();

    /**
     * Exit all worker threads
     */
    void ExitAll();

protected:

    typedef std::vector<Thread*> ThreadVec;

    ThreadVec threads;
};

/**
 * Work
 */
class Work {
public:
    /**
     * Constructor
     */
    Work() {}
    /**
     * Destructor
     */
    virtual ~Work() {}
    
    /**
     * Is need delete
     */
    virtual bool NeedDelete() const = 0;

    /**
     * Do work
     * @return 0 if success, -1 if fail
     */
    virtual int DoWork() = 0;
};

/**
 * Thread-safe WorkQueue
 */
class WorkQueue {
public:
    /**
     * Constructor
     */
    WorkQueue();

    /**
     * Destructor
     */
    ~WorkQueue();

    /**
     * Add one work to WorkQueue and wake up one thread
     * @param work Work
     * @return 0 if success, -1 if fail
     */
    int AddWork(Work* work);

    /**
     * Get one work from WorkQueue and wait if empty
     * Wait if WorkQueue is empty
     * @return Work
     */
    Work* GetWork();

    /**
     * Shutdown WorkQueue
     * @return 0 if success, -1 if fail
     */
    int Shutdown();

    /**
     * Is shutdown
     */
    bool IsShutdown();

private:

    typedef std::deque<Work*> Queue;

    pthread_mutex_t mutex;

    pthread_cond_t cond;

    bool shutdown;

    Queue works;
};

/**
 * Worker thread get works from WorkQueue and do works
 */
class Worker : public Runnable {
public:
    /**
     * Constructor
     */
    Worker(WorkQueue& queue);

    /**
     * Destructor
     */
    virtual ~Worker();

	/**
     * Get works from WorkQueue and do works
     * @return 0 if success, -1 if fail
     */
    virtual int Run();
    
private:
    
    WorkQueue& workQueue;
};

/**
 * A number of Worker threads get works from WorkQueue and do works
 */
class WorkerThreadPool {
public:
    /**
     * Constructor
     */
    WorkerThreadPool();

    /**
     * Destructor
     */
    ~WorkerThreadPool();

    /**
     * Start a WorkerThreadPool
     * @param threadCount WorkerThreadPool size
     * @return 0 if success, -1 if fail
     */
    int Start(int threadCount);

    /**
     * Add one work to WorkQueue and wake up worker threads
     * @param work work
     * @return 0 if success, -1 if fail
     */
    int AddWork(Work* work);

    /**
     * Shutdown WorkerThreadPool
     */
    int Shutdown();

    /**
     * Wait for all worker threads to finish and reap them
     * @return 0 if success, -1 if fail
     */
    int JoinAll();

    /**
     * Exit all worker threads
     */
    void ExitAll();

protected:

    /**
     * Start a WorkerThreadPool
     * @param threadCount WorkerThreadPool size
     * @param target task function
     * @return 0 if success, -1 if fail
     */
    int Start(int threadCount, Runnable& target);

    WorkQueue workQueue;

    Worker worker;

    ThreadPool pool;
};

} // mmtraining

#endif // MM_TRANNING_THREAD_POOL_H
