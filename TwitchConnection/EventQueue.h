#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H
#include <mutex>
#include "User.hpp"

struct updateEvent {
    enum ACTION {
        NEW_FOLLOWER,
        CHANGE_COLOR,
        CHANGE_SHAPE,
        NEW_SUBSCRIBER,
        GIFTED_SUBSCRIBER,
        NONE
    };
    ACTION action = NONE;
    User* user = nullptr;
    User* gifter = nullptr;
    ~updateEvent(){
        if(user)
            delete user;
        if(gifter)
            delete gifter;
    }
};

class Node {
public:
    Node* next = nullptr;
    updateEvent* item = nullptr;
    Node(updateEvent* p_item): item(p_item){};
};

class EventQueue {
public:
    EventQueue();
    ~EventQueue();
    void push(updateEvent* event);
    updateEvent* pop();
    updateEvent* peek();
    int getSize();
    bool isEmpty();
    std::mutex mut;
private:
    Node* head = nullptr;
    Node* tail = nullptr;
    int size = 0;
};
#endif