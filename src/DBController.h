#ifndef DBHANDLER_H
#define DBHANDLER_H
#include <iostream>
#include <mysql/jdbc/mysql_driver.h>
#include <mysql/jdbc/mysql_connection.h>
#include <mysql/jdbc/cppconn/driver.h>
#include <mysql/jdbc/cppconn/exception.h>
#include <mysql/jdbc/cppconn/resultset.h>
#include <mysql/jdbc/cppconn/statement.h>
#include <mysql/jdbc/cppconn/prepared_statement.h>

#include <thread>
#include <mutex>

#include "User.hpp"
#include "EventQueue.h"
#include "Semaphore.hpp"

#define host ""
#define usr ""
#define port 3306
#define db "userDB"
#define password ""


using namespace sql;
class DBController {
public:
    DBController();
    ~DBController();
    void getUsers(UserDict* users);
    static void addUser(User* user, Connection* con);
    void processEvent(updateEvent* event);
    
private:
    std::thread* handlerThread;
    static void handler(EventQueue* eventQueue, Connection* con);
    EventQueue* eventQueue;
    static std::mutex eventQueueMux;
    static Semaphore sem;
    mysql::MySQL_Driver* driver;
    Connection* con;

};
#endif