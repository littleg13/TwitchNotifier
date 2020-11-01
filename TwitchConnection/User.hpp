#ifndef USER_H
#define USER_H

#include "json11.hpp"
#include <mysql/jdbc/cppconn/resultset.h>
#include <algorithm>
#include <unordered_map>

class User {
    public:
	int id;
	std::string name;
    int index;
	float color[3] = {0,0,0};
	bool hasColor = false;
    int privilege = 0;
    std::string shape = "cube";

    User(int userID, std::string userName): id(userID), name(userName){}

    User(const User &user){
        id = user.id;
        name = user.name;
        index = user.index;
        hasColor = user.hasColor;
        for(int i=0;i<3;i++)
            color[i] = user.color[i];
        shape = user.shape;
        privilege = user.privilege;
    }

    User(sql::ResultSet* res){
        id = res->getInt("userID");
        index = res->getInt("index");
        name = res->getString("userName");
        privilege = res->getInt("privilege");
        hasColor = res->getBoolean("hasColor");
        color[0] = res->getDouble("colorR");
        color[1] = res->getDouble("colorG");
        color[2] = res->getDouble("colorB");
        shape = res->getString("shape");
    }

    User(json11::Json json){
        if(json.object_items().count("from_id") == 1){
            id = stoi(json["from_id"].string_value());
            name = json["from_name"].string_value();
            privilege = 1;
        }
        else{
            id = stoi(json["user_id"].string_value());
            name = json["user_name"].string_value();
            privilege = 1 + (stoi(json["tier"].string_value()) / 1000);
        }
        
        transform(name.begin(), name.end(), name.begin(), ::tolower);
    };

    json11::Json to_json(){
        json11::Json::object ob = json11::Json::object({
            {"id", json11::Json(id)},
            {"name", json11::Json(name)},
            {"color", json11::Json(std::vector<float>(color, color + 3))},
            {"hasColor", json11::Json(hasColor)},
            {"shape", json11::Json(shape)},
            {"index", json11::Json(index)}
            });
        json11::Json::object oob = json11::Json::object({
            {name, ob}
        });
        return json11::Json(oob);
    };
};
typedef std::unordered_map<int, User*> UserDict;
static int getUserID(std::string userName, UserDict* users){
    //TODO NEEDS REWORK!
    for(auto it=users->begin();it!=users->end();it++){
        if(it->second->name == userName)
            return it->first;
    }
    return -1;
}
#endif