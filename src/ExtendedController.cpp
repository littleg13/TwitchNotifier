// ExtendedController.c++

#include "ExtendedController.h"
#include "ModelView.h"

ChatBot* ExtendedController::chatBot = nullptr;
NotifyServer* ExtendedController::notifyServer = nullptr;
SoundController* ExtendedController::soundController = nullptr;

ExtendedController::ExtendedController(const std::string& name, int rcFlags):
	GLFWController(name, rcFlags)
{
	followers = new std::unordered_map<std::string, Follower*>;
}

ExtendedController::~ExtendedController(){
	delete followers;
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
		if(event->action == updateEvent::ACTION::NEW_FOLLOWER){
			updateFollowers((*(event->info))["data"].array_items());
		}
		for (std::vector<ModelView*>::iterator it=models.begin() ; it<models.end() ; it++)
			(*it)->handleUpdate(reinterpret_cast<void*>(event));

		chatBot->processEvent(event);
		soundController->processEvent(event);
		delete event;
		writeFollowersToFile();
	}
}

void ExtendedController::startTwitchServer(EventQueue* eventQueue){
    notifyServer = new NotifyServer(eventQueue);
	notifyServer->run();
}

void ExtendedController::startChatServer(EventQueue* eventQueue, followerDict* followers){
	chatBot = new ChatBot(eventQueue, followers);
	chatBot->runWebSocket();
}

void ExtendedController::writeFollowersToFile(){
	std::ofstream file_handle;
	file_handle.open(followerFile, std::ofstream::trunc);
	file_handle << "{\n";
	std::string toPrint = "";
	for(auto it = followers->begin(); it != followers->end(); ++it){
		std::string jsonString = (*it).second->to_json().dump();
		toPrint += jsonString.substr(1, jsonString.length() - 2) + ",\n";
	}
	file_handle << toPrint.substr(0, toPrint.length()-2);
	file_handle << "\n}";
	file_handle.close();
}

void ExtendedController::updateFollowers(std::vector<json11::Json> follower_arr){
    for(auto it = follower_arr.begin();it != follower_arr.end(); ++it){
		std::string name = (*it)["from_name"].string_value();
		transform(name.begin(), name.end(), name.begin(), ::tolower);
		std::string followerName = name;
		if(followers->find(followerName) == followers->end()){
			followers->insert({followerName, new Follower(*it)});
			(*followers)[followerName]->index = numFollowers;
			numFollowers++;
		}
    }
}

void ExtendedController::readFollowersFromFile(){
	std::ifstream file_handle;
	file_handle.open(followerFile);
	std::string content((std::istreambuf_iterator<char>(file_handle)), (std::istreambuf_iterator<char>()));
	file_handle.close();
	std::string err;
	json11::Json json = json11::Json::parse(content, err);
	const json11::Json::object* ob = &(json.object_items());
	for(auto it = ob->begin(); it != ob->end(); ++it){
		Follower* fol = new Follower(it->second);
		followers->insert({it->first, fol});
	}
	numFollowers = followers->size();
}

void ExtendedController::enableTwitch(){
	eventQueue = new EventQueue();
	soundController = new SoundController();
    con = new TwitchConnection();
    con->getOauthToken();
    twitchThread = new std::thread(ExtendedController::startTwitchServer, eventQueue);
	chatThread = new std::thread(ExtendedController::startChatServer, eventQueue, followers);
	readFollowersFromFile();
    con->subscribeToFollower();
	updateFollowers(con->getFollowers());
	
}

// The following must a public method in class ExtendedController:
bool ExtendedController::drawingOpaque() const // CALLED FROM SceneElement or descendant classes
{
    return drawingOpaqueObjects;
}