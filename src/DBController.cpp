#include "DBController.h"

std::mutex DBController::eventQueueMux;
Semaphore DBController::sem;

DBController::DBController(){
    driver = mysql::get_mysql_driver_instance();
    con = driver->connect(host, usr, password);
    con->setSchema("userDB");
    eventQueue = new EventQueue();
    handlerThread = new std::thread(handler, eventQueue, con);
}

void DBController::handler(EventQueue* eventQueue, Connection* con){
    while(1){
        sem.aqquire();
        eventQueueMux.lock();
        updateEvent* event = eventQueue->pop();
        eventQueueMux.unlock();
        addUser(event->user, con);
        event->user = nullptr;
        delete event;
    }
}

void DBController::processEvent(updateEvent* event){
    if(event->user){
        eventQueueMux.lock();
        eventQueue->push(event);
        eventQueueMux.unlock();
        sem.release();
    }
}

DBController::~DBController(){
    delete con;
    delete eventQueue;
}

void DBController::getUsers(UserDict* users){
    Statement* query = con->createStatement();
    ResultSet* res = query->executeQuery("SELECT * FROM users");
    while(res->next()){
        User* user = new User(res);
        user->index = users->size();
        users->insert({user->id, user});
    }
}

void DBController::addUser(User* user, Connection* con){
    try{
    PreparedStatement* query = con->prepareStatement(
        "INSERT INTO users (`index`, userID, userName, privilege, hasColor, colorR, colorG, colorB, shape) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?) " 
        "ON DUPLICATE KEY UPDATE userName=VALUES(userName), privilege=VALUES(privilege), hasColor=VALUES(hasColor), "
        "colorR=VALUES(colorR), colorG=VALUES(colorG), colorB=VALUES(colorB), shape=VALUES(shape)");
    query->setInt(1, user->index);
    query->setInt(2, user->id);
    query->setString(3, user->name);
    query->setInt(4, user->privilege);
    query->setBoolean(5, user->hasColor);
    query->setDouble(6, user->color[0]);
    query->setDouble(7, user->color[1]);
    query->setDouble(8, user->color[2]);
    query->setString(9, user->shape);
    query->executeUpdate();
    }
    catch(SQLException &e){
        std::cout << e.what() << std::endl;
    }
}