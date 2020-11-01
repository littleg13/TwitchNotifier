// ExtendedController.h

#ifndef EXTENDEDCONTROLLER_H
#define EXTENDEDCONTROLLER_H

#include <thread>
#include "GLFWController.h"
#include "NotifyServer.h"
#include "TwitchConnection.h"
#include "ChatBot.h"
#include "Sound/SoundController.h"
#include "DBController.h"

#include <fstream>


class ExtendedController : public GLFWController
{
protected:
	void handleMouseMotion(int x, int y);
	void handleDisplay();
	static void startTwitchServer(EventQueue* eventQueue, UserDict* users);
	static void startChatServer(EventQueue* eventQueue, UserDict* users);
	static ChatBot* chatBot;
	static NotifyServer* notifyServer;
	static SoundController* soundController;
	void notifyOnUpdate();
	EventQueue* eventQueue;
	std::thread* twitchThread;
	std::thread* chatThread;
	TwitchConnection* con;
	DBController* dbController;
public:
	void enableTwitch();
	ExtendedController(const std::string& name, int rcFlags = 0);
	~ExtendedController();
	bool drawingOpaque() const;
	void updateUsers(User** user_arr, int n);
	UserDict* users;
	int numusers = 0;
private:
	bool drawingOpaqueObjects = false;
	void saveUserDB(int userID);
	void loadUserDB();
	const std::string followerFile = "users.json";
	
};

#endif
