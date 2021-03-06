#ifndef FOOTERTEXT_H
#define FOOTERTEXT_H

#include "SceneElement.h"
#include "GLFWController.h"
#include "CFont.h"
#include "CGLString.h"

class FooterText : public SceneElement {
public:
    FooterText(ShaderIF* sIF, UserDict* p_users);
	virtual ~FooterText();
	void getMCBoundingBox(double* xyzLimits) const;
	bool handleCommand(unsigned char anASCIIChar, double ldsX, double ldsY);
	void render();
	void renderFooterText(cryph::Matrix4x4 M);
	bool handleUpdate(void* update);
private:
    void createString(std::string msg);
    float getOpacity();
    float startTime = 0;
    float textTimeout = 4.0;
    CFont* font;
    CGLString* glString;
    std::string fontFile = "fonts/AlbertusMedium12.fnt";

};
#endif