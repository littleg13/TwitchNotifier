#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H
#include "json11.hpp"

struct updateEvent {
    enum ACTION {
        NEW_FOLLOWER,
        CHANGE_COLOR,
        NONE
    };
    ACTION action = NONE;
    json11::Json* info;
    void reset(){
        action = NONE;
        info = nullptr;
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
private:
    Node* head = nullptr;
    Node* tail = nullptr;
    int size = 0;
};
#endif