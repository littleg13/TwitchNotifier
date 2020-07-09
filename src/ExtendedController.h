// ExtendedController.h

#ifndef EXTENDEDCONTROLLER_H
#define EXTENDEDCONTROLLER_H

#include <thread>
#include "GLFWController.h"
#include "NotifyServer.h"
#include "TwitchConnection.h"
#include "ChatBot.h"

#include <fstream>

typedef std::unordered_map<std::string, Follower*> followerDict;


class ExtendedController : public GLFWController
{
protected:
	void handleMouseMotion(int x, int y);
	void handleDisplay();
	static void startTwitchServer(updateEvent* update);
	static void startChatServer(updateEvent* update, followerDict* followers);
	void notifyOnUpdate();
	updateEvent* update;
	std::thread* twitchThread;
	std::thread* chatThread;
	TwitchConnection* con;
public:
	void enableTwitch();
	ExtendedController(const std::string& name, int rcFlags = 0);
	~ExtendedController();
	bool drawingOpaque() const;
	void updateFollowers(std::vector<json11::Json> follower_arr);
	followerDict* followers;
	int numFollowers = 0;
private:
	bool drawingOpaqueObjects = false;
	void writeFollowersToFile();
	void readFollowersFromFile();
	const std::string followerFile = "followers.json";
	
};

#endif
