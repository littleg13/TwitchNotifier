#include "Semaphore.h"

Semaphore::Semaphore(){

}
Semaphore::~Semaphore(){

}
void Semaphore::aqquire(){
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
}
void Semaphore::release(){
    counterMux.lock();
    counter++;
    counterMux.unlock();
    cv.notify_one();
}