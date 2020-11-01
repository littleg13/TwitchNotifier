// ExtendedController.c++

#include "ExtendedController.h"
#include "ModelView.h"

ChatBot* ExtendedController::chatBot = nullptr;
NotifyServer* ExtendedController::notifyServer = nullptr;
SoundController* ExtendedController::soundController = nullptr;

ExtendedController::ExtendedController(const std::string& name, int rcFlags):
	GLFWController(name, rcFlags)
{
	users = new UserDict;
}

ExtendedController::~ExtendedController(){
	delete users;
	delete eventQueue;
}

void ExtendedController::handleMouseMotion(int x, int y)
{
	int dx = x - screenBaseX;
	int dy = y - screenBaseY;
	screenBaseX = x;
	screenBaseY = y;
	if(mouseMotionIsTranslate){
		double ldsX, ldsY;
		pixelVectorToLDSVector(dx, dy, ldsX, ldsY);
		ModelView::addToGlobalPan(ldsX, ldsY, 0);
	}
	else if(mouseMotionIsRotate){
		double rotFactor = 1;
		double rotX = rotFactor * dy;
		double rotY = rotFactor * dx;
		ModelView::addToGlobalRotationDegrees(rotX, rotY, 0);
	}
	redraw();
}

void ExtendedController::handleDisplay()
{
	notifyOnUpdate();
    prepareWindow(); // an inherited GLFWController method needed for some platforms
    glDepthMask(GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw opaque objects
    glDisable(GL_BLEND);
    drawingOpaqueObjects = true; // record in instance variable so ModelView instances can query
    renderAllModels();

    // draw translucent objects
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    drawingOpaqueObjects = false; // record in instance variable so ModelView instances can query
    renderAllModels();

    swapBuffers();
}

void ExtendedController::notifyOnUpdate(){
	if(!eventQueue->isEmpty()){
		eventQueue->mut.lock();
		updateEvent* event = eventQueue->pop();
		eventQueue->mut.unlock();
		if(event->action == updateEvent::ACTION::NEW_FOLLOWER ||
		   event->action == updateEvent::ACTION::NEW_SUBSCRIBER ||
		   event->action == updateEvent::ACTION::GIFTED_SUBSCRIBER){
			User* user_arr[1] = {event->user};
			updateUsers(user_arr, 1);
		}
		for (std::vector<ModelView*>::iterator it=models.begin() ; it<models.end() ; it++)
			(*it)->handleUpdate(reinterpret_cast<void*>(event));

		chatBot->processEvent(event);
		soundController->processEvent(event);
		if(event->user){
			saveUserDB(event->user->id);
		}
		delete event;
	}
}

void ExtendedController::startTwitchServer(EventQueue* eventQueue, UserDict* users){
    notifyServer = new NotifyServer(eventQueue, users);
	notifyServer->run();
}

void ExtendedController::startChatServer(EventQueue* eventQueue, UserDict* users){
	chatBot = new ChatBot(eventQueue, users);
	chatBot->runWebSocket();
}

void ExtendedController::saveUserDB(int userID){
	updateEvent* subEvent = new updateEvent();
	subEvent->user = users->at(userID);
	dbController->processEvent(subEvent);
}

void ExtendedController::updateUsers(User** user_arr, int n){
    for(int i=0;i<n;i++){
		if(users->find(user_arr[i]->id) == users->end()){
			User* user = new User(*user_arr[i]);
			user->index = numusers;
			users->insert({user->id, user});
			numusers++;
		}
		else{
			if(user_arr[i]->privilege > users->at(user_arr[i]->id)->privilege)
				users->at(user_arr[i]->id)->privilege = user_arr[i]->privilege;
		}
    }
}

void ExtendedController::loadUserDB(){
	dbController->getUsers(users);
	numusers = users->size();
}

void ExtendedController::enableTwitch(){
	dbController = new DBController();
	eventQueue = new EventQueue();
	soundController = new SoundController();
    con = new TwitchConnection();
    con->getOauthToken();
    twitchThread = new std::thread(ExtendedController::startTwitchServer, eventQueue, users);
	chatThread = new std::thread(ExtendedController::startChatServer, eventQueue, users);
	loadUserDB();
    con->subscribeToFollower();
	con->subscribeToSubscriber();
	std::vector<User*>* userVec = new std::vector<User*>();
	con->getUsers(userVec);
	updateUsers(userVec->data(), userVec->size());
	delete userVec;
}

// The following must a public method in class ExtendedController:
bool ExtendedController::drawingOpaque() const // CALLED FROM SceneElement or descendant classes
{
    return drawingOpaqueObjects;
}