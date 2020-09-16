#include "EventQueue.h"

EventQueue::EventQueue(): size(0), head(nullptr), tail(nullptr){}

EventQueue::~EventQueue(){
    while(!isEmpty()){
        pop();
    }
}

void EventQueue::push(updateEvent* event){
    Node* newNode = new Node(event);
    if(isEmpty()){
        head = newNode;
        tail = newNode;
    }
    else{
        tail->next = newNode;
        tail = newNode;
    }
    size++;
}

updateEvent* EventQueue::pop(){
    if(isEmpty())
        return nullptr;
    Node* toReturn = head;
    head = head->next;
    size--;
    return toReturn->item;
}

updateEvent* EventQueue::peek(){
    return head->item;
}

int EventQueue::getSize(){
    return size;
}

bool EventQueue::isEmpty(){
    return !size;
}