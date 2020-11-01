#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#include <mutex>
#include <condition_variable>

class Semaphore {
public:
    Semaphore(){};
    ~Semaphore(){};
    void aqquire(){
        std::unique_lock<std::mutex> ul(cvMux);
        counterMux.lock();
        int currentCount = counter;
        counterMux.unlock();
        if(currentCount == 0){
            cv.wait(ul);
        }
        counterMux.lock();
        counter--;
        counterMux.unlock();
    };
    void release(){
        counterMux.lock();
        counter++;
        counterMux.unlock();
        cv.notify_one();
    };
private:
    int counter = 0;
    std::mutex counterMux;
    std::mutex cvMux;
    std::condition_variable cv;

};
#endif