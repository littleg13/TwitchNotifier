#include "FooterText.h"

FooterText::FooterText(ShaderIF* sIF, UserDict* p_users): SceneElement(sIF){
    font = CFont::getFont(fontFile);
    createString("");
}

FooterText::~FooterText(){
}

void FooterText::createString(std::string msg){
    glString = new CGLString(msg, font, 2);
    glString->setStringDimensions(10.0, 1.0);
    glString->setStringOrigin(glString->getCurrentRenderWidth() / (-2.0), -6.0, 0.0);
    double dir[3];
    cryph::AffVector eyeDir = eye;
    eyeDir.cross(-cryph::AffVector::yu).vComponents(dir);
    glString->setStringDirection(dir[0], dir[1], dir[2]);
}
void FooterText::getMCBoundingBox(double* xyzLimits) const{
    xyzLimits[0] = 0.0;
    xyzLimits[1] = 0.0;
    xyzLimits[2] = 0.0;
    xyzLimits[3] = 0.0;
    xyzLimits[4] = 0.0;
    xyzLimits[5] = 0.0;
}
bool FooterText::handleCommand(unsigned char anASCIIChar, double ldsX, double ldsY){
    return true;
}
void FooterText::render(){
    GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);
    glUseProgram(shaderIF->getShaderPgmID());
    cryph::Matrix4x4 mc_ec, ec_lds;
	getMatrices(mc_ec, ec_lds);
	float mat[16];
	glUniformMatrix4fv(shaderIF->ppuLoc("ec_lds"), 1, false, ec_lds.extractColMajor(mat));
    establishLightingEnvironment();
	renderFooterText(mc_ec);
}

float FooterText::getOpacity(){
    float currentTime = glfwGetTime() - startTime;
    float percentTime = currentTime/textTimeout;
    if(percentTime < 1.0)
        return min(4 * sin((percentTime) * 3.14), 1.0);
    else
        return 0;
}

void FooterText::renderFooterText(cryph::Matrix4x4 M){
    float mat[16];
    glUniformMatrix4fv(shaderIF->ppuLoc("mc_ec"), 1, false, M.extractColMajor(mat));
    float fColor[] = { 1.0, 1.0, 1.0, getOpacity() }; // color to be used to render the font
	glUniform4fv(shaderIF->ppuLoc("fontColor"), 1, fColor);
    glUniform1i(shaderIF->ppuLoc("renderingFontString"), 1);
    glString->renderString(shaderIF->pvaLoc("mcPosition"), shaderIF->pvaLoc("texCoords"), shaderIF->ppuLoc("fontTextureMap"));
    glUniform1i(shaderIF->ppuLoc("renderingFontString"), 0);
}
bool FooterText::handleUpdate(void* update){
    updateEvent* event = reinterpret_cast<updateEvent*>(update);
	if(event->action == updateEvent::ACTION::NEW_FOLLOWER){
		createString("Thanks " + event->user->name + " for following!");
        startTime = glfwGetTime();
	}
    else if(event->action == updateEvent::ACTION::NEW_SUBSCRIBER){
        createString("Thanks " + event->user->name + " for subscribing!");
        startTime = glfwGetTime();
    }
    else if(event->action == updateEvent::ACTION::GIFTED_SUBSCRIBER){
        createString(event->gifter->name + " gifted a sub to " + event->user->name);
        startTime = glfwGetTime();
    }
    return false;
}