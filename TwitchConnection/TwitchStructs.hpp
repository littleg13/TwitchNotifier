#ifndef TWITCHSTRUCTS_H
#define TWITCHSTRUCTS_H

#include "json11.hpp"
#include <algorithm>

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

struct Follower {
	std::string id;
	std::string name;
    int index;
	float color[3] = {0,0,0};
	bool hasColor = false;

    Follower(json11::Json json){
        if(json.object_items().count("from_id") == 1){
            id = json["from_id"].string_value();
            name = json["from_name"].string_value();
        }
        else{
            id = json["id"].string_value();
            name = json["name"].string_value();
            hasColor = json["hasColor"].bool_value();
            if(hasColor){
                for(int i=0;i<3;i++){
                    color[i] = json["color"].array_items()[i].number_value();
                }
            }
            index = json["index"].number_value();
        }
        
        transform(name.begin(), name.end(), name.begin(), ::tolower);
    };

    json11::Json to_json(){
        json11::Json::object ob = json11::Json::object({
            {"id", json11::Json(id)},
            {"name", json11::Json(name)},
            {"color", json11::Json(std::vector<float>(color, color + 3))},
            {"hasColor", json11::Json(hasColor)},
            {"index", json11::Json(index)}
            });
        json11::Json::object oob = json11::Json::object({
            {name, ob}
        });
        return json11::Json(oob);
    };
};
typedef std::unordered_map<std::string, Follower*> followerDict;

#endif