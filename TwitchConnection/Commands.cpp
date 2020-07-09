#include "Commands.h"


commandMap Commands::Map {
        {"color", changeColor}
    };

updateEvent* Commands::update = nullptr;
followerDict* Commands::followers = nullptr;


int Commands::changeColor(Command com, std::string &err){
    if(followers->find(com.user) == followers->end()){
        err = "You must be following to have a cube";
        return 1;
    }
    if(com.data.length() > 0){
        int pos;
        std::string color_string[3];
        float color[3];
        int count = 0;
        while((pos = com.data.find(" ")) != com.data.npos){
            if(count >= 2){
                err = "Too many values";
                return 1;
            }
            color_string[count] = com.data.substr(0, pos);
            com.data.erase(0, pos + 1);
            count++;
        }
        color_string[count] = com.data;
        if(count < 2){
            err = "Not enough values";
            return 1;
        }
        for(int i=0;i<3;i++){
            try{
                color[i] = std::stof(color_string[i], nullptr);
            }
            catch(std::invalid_argument &e){
                err = color_string[i] + " is not a number";
                return 1;
            }
            if(color[i] < 0 || color[i] > 1){
                err = color_string[i] + " is not between 0 and 1";
                return 1;
            }
        }
        for(int i=0;i<3;i++){
            (*followers)[com.user]->color[i] = color[i];
            (*followers)[com.user]->hasColor = true;
        }
        update->info = new json11::Json(
            json11::Json::object({{"user", com.user}})
        );
        update->action = updateEvent::ACTION::CHANGE_COLOR;
    }
    else{
        err = "Supply a color in the form of !color R G B\r\n0 < R,G,B < 1";
        return 1;
    }
    return 0;
}