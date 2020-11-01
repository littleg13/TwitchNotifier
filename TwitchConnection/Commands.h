#ifndef COMMANDS_H
#define COMMANDS_H
#include <string>
#include <unordered_map>
#include <stdexcept>
#include "user.hpp"
#include "EventQueue.h"

#define TotalShapes 4

struct Command{
    int userID;
    std::string command = "";
    std::string data = "";
};

typedef int (*commandFunction)(Command com, std::string &err);
typedef std::unordered_map<std::string, commandFunction> commandMap;
class Commands{
public:
    static int changeColor(Command com, std::string &err);
    static int changeShape(Command com, std::string &err);
    static int displayShapes(Command com, std::string &err);
    static std::string knownShapes[TotalShapes];
    static EventQueue* eventQueue;
    static UserDict* users;
    static commandMap Map;
};
#endif