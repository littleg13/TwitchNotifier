#ifndef COMMANDS_H
#define COMMANDS_H
#include <string>
#include <unordered_map>
#include <stdexcept>
#include "Follower.hpp"
#include "EventQueue.h"

struct Command{
    std::string user = "";
    std::string command = "";
    std::string data = "";
};

typedef int (*commandFunction)(Command com, std::string &err);
typedef std::unordered_map<std::string, commandFunction> commandMap;
typedef std::unordered_map<std::string, Follower*> followerDict;

class Commands{
public:
    static int changeColor(Command com, std::string &err);
    static EventQueue* eventQueue;
    static followerDict* followers;
    static commandMap Map;
};
#endif