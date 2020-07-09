// SceneElement.h - a base class that factors out many common data and
//                  method implementations to support a Phong local
//                  lighting model.

#ifndef SCENEELEMENT_H
#define SCENEELEMENT_H

#include "GLFWController.h"
#include "ModelView.h"
#include "PhongMaterial.h"
#include "ExtendedController.h"
#include "ShaderIF.h"

// FOLLOWING CANNOT BE LARGER THAN "MAX_NUM_LIGHTS" IN the shader program
static const int MAX_NUM_LIGHTS = 3;

class SceneElement : public ModelView
{
public:
	SceneElement(ShaderIF* sIF);
	virtual ~SceneElement();

	static GLuint readTextureImage(const std::string& imgFileName);
	bool handleCommand(unsigned char anASCIIChar, double ldsX, double ldsY);
	bool handleUpdate(void* update);
	virtual void render(cryph::Matrix4x4 M){};

protected:
	ShaderIF* shaderIF;

	void establishLightingEnvironment( /* ... parameters? ... */ );
	void establishMaterial(const PhongMaterial& matl);
	void establishTexture(GLuint texID, bool useAlpha, int mode);
	float establishSeason();
	void establishView(cryph::Matrix4x4 M);

	float last_time = -1.0;
	bool paused = false;
	float total_time = 0;
	bool useTexture = false;

	// lighting environment
	static float lightPos[4*MAX_NUM_LIGHTS]; // (x,y,z,w) for each light
	static bool posInModelCoordinates[MAX_NUM_LIGHTS]; // pos is in MC or EC?
	static int enabled[MAX_NUM_LIGHTS]; // is this light turned on?
	static float lightStrength[3*MAX_NUM_LIGHTS]; // (r,g,b) for each light
	static float globalAmbient[3]; // (r,g,b) for ambient term, A
};

#endif
