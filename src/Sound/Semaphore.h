#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#include <mutex>
#include <condition_variable>

class Semaphore {
public:
    Semaphore();
    ~Semaphore();
    void aqquire();
    void release();
private:
    int counter = 0;
    std::mutex counterMux;
    std::mutex cvMux;
    std::condition_variable cv;

};
#endif