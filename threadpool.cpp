
#include "threadpool.h"
#include <iostream>

/**
 * Thread pool
 * @author ianhua
 */

//extern int remainWork;

namespace mmtraining {

/************************Thread***************************/

Thread::Thread() : tid(0), running(false), target(NULL) {}

Thread::Thread(Runnable& t) : tid(0), running(false), target(&t) {}

Thread::~Thread() {
    Join();
}

void* Thread::pthread_func(void* args) {
    // Cast args to Thread pointer
    Thread* thread = (Thread*)args;
    thread->Run();
    return NULL;
}

int Thread::Start() {
    // Create a thread
    if (pthread_create(&tid, NULL, pthread_func, (void*)this) != 0) {
        return -1;
    }
    running = true;
    std::cout << "Start a new thread: " << tid << std::endl;
    return 0;
}

pthread_t Thread::GetId() const {
    return tid;
}

int Thread::Run() {
    int ret = -1;
    if (target != NULL) {
        ret = target->Run();
    } else { 
        ret = this->DoRun();
    }
    running = false;
    return ret;
}

int Thread::DoRun() {
    std::cout << "Thread DoRun!" << std::endl;
    return 0;
}

bool Thread::IsRunning() const {
    return running;
}

int Thread::Join() {
    // Join thread tid
    if (pthread_join(tid, NULL) != 0) {
        return -1;
    }
    return 0;
}

void Thread::Exit() {
    pthread_exit((void*)1);
}

/***********************ThreadPool***********************/

ThreadPool::ThreadPool() {}

ThreadPool::~ThreadPool() {
    // Delete all thread pointers and clear vector
    for (int i = 0; i < threads.size(); i++) {
        delete threads[i];
    }
    threads.clear();
}

int ThreadPool::Start(int threadCount, Runnable& target) {
    if (threadCount == 0) {
        return -1;
    }
    Thread* thread;
    for (int i = 0; i < threadCount; i++) {
        // Create thread pointers
        thread = new Thread(target);
        if (thread == NULL) {
            return -1;
        }
        threads.push_back(thread);

        // Run these threads
        if (thread->Start() != 0) {
            return -1;
        }
    }

    return 0;
}

int ThreadPool::JoinAll() {
    for (int i = 0; i < threads.size(); i++) {
        if (threads[i]->Join() != 0) {
            return -1;
        }
    }
    return 0;
}

void ThreadPool::ExitAll() {
    for (int i = 0; i < threads.size(); i++) {
        threads[i]->Exit();
    }
}

/**********************WorkQueue*************************/

WorkQueue::WorkQueue() : shutdown(false) {
    // Initialize mutex and cond
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        std::cout << "mutex initialization: FAIL!" << std::endl;
    }
    if (pthread_cond_init(&cond, NULL) != 0) {
        std::cout << "cond initialization: FAIL!" << std::endl;
    }
}

WorkQueue::~WorkQueue() {
    // Destroy mutex and cond
    if (pthread_mutex_destroy(&mutex) != 0) {
        std::cout << "mutex destroy: FAIL!" << std::endl;
    }
    if (pthread_cond_destroy(&cond) != 0) {
        std::cout << "cond destroy: FAIL!" << std::endl;
    }
    for (int i = 0; i < works.size(); i++) {
        delete works[i];
    }
    works.clear();
}

int WorkQueue::AddWork(Work* work) {

    if (work == NULL || IsShutdown()) {
        return -1;
    }

    // Lock
    if (pthread_mutex_lock(&mutex) != 0) {
        return -1;
    }

    if (works.size() >= works.max_size()) {
        return -1;
    }

    std::cout << "Add work..." << std::endl;

    works.push_back(work);

    // Unlock
    if (pthread_mutex_unlock(&mutex) != 0) {
        return -1;
    }

    // Wake up one thread
    if (pthread_cond_signal(&cond) != 0) {
        return -1;
    }

    return 0;
}

Work* WorkQueue::GetWork() {
  
    if (pthread_mutex_lock(&mutex) != 0) {
        return NULL;
    }

    // Wait if empty and not shutdown
    while (!IsShutdown() && works.empty()) {
        //std::cout << "Getting work..." << std::endl;
        pthread_cond_wait(&cond, &mutex);
        //std::cout << "Got work..." << std::endl;
    }

    if (IsShutdown()) {
        if (pthread_mutex_unlock(&mutex) != 0) {
            return NULL;
        }
        return NULL;
    }

    Work* work = works.front();

    works.pop_front();

    if (pthread_mutex_unlock(&mutex) != 0) {
        return NULL;
    }

    return work;
}

int WorkQueue::Shutdown() {
    if (IsShutdown()) {
        return 0;
    }
    shutdown = true;

    // Wake up all threads
    if (pthread_cond_broadcast(&cond) != 0) {
        return -1;
    }

    std::cout << "Shutting down..." << std::endl;

    return 0;
}

bool WorkQueue::IsShutdown() {
    return shutdown;
}

/**********************Worker*******************************/

Worker::Worker(WorkQueue& queue) : workQueue(queue) {}

Worker::~Worker() {}

int Worker::Run() {
    // Loop until workQueue shutdown
    Work* work;
    while (!workQueue.IsShutdown()) {
        // Get work
        if ((work = workQueue.GetWork()) != NULL) {
            work->DoWork();
            if (work->NeedDelete()) {
                delete work;
            }
            //remainWork = remainWork - 1;
            //std::cout << "Remain: " << remainWork << std::endl;
        }
    }
    return 0;
}

/**********************WorkerThreadPool***********************/

WorkerThreadPool::WorkerThreadPool() : worker(workQueue) {}

WorkerThreadPool::~WorkerThreadPool() {}

int WorkerThreadPool::Start(int threadCount) {
    return pool.Start(threadCount, worker);
}

int WorkerThreadPool::AddWork(Work* work) {
    return workQueue.AddWork(work);
}

int WorkerThreadPool::Shutdown() {
    return workQueue.Shutdown();
}

int WorkerThreadPool::JoinAll() {
    return pool.JoinAll();
}

void WorkerThreadPool::ExitAll() {
    return pool.ExitAll();
}

} // mmtraining


