// SceneElement.c++

#include "SceneElement.h"

float SceneElement::lightPos[4*MAX_NUM_LIGHTS] =
	{
		5.0, 5, 5.0, 1.0,
		0.0, 0.0, 5, 1.0,
		0.0, 0.0, 5.0, 1.0
	};

// Are coordinates in "lightPos" stored in MC or EC? (You usually want them
// defined in MC, unless you are trying to simulate something like the viewer
// holding a flashlight while moving around the scene.)
bool SceneElement::posInModelCoordinates[MAX_NUM_LIGHTS] =
	{ true, true, true };
// Is the light currently enabled?
int SceneElement::enabled[MAX_NUM_LIGHTS] =
	{ 1, 1, 1 };
// The following is the buffer actually sent to GLSL. It will contain a copy of
// the (x,y,z,w) for light sources defined in EC; it will contain the coordinates
// after transformation to EC if the position was originally specified in MC.
float posToGLSL[4*MAX_NUM_LIGHTS];

float SceneElement::lightStrength[3*MAX_NUM_LIGHTS] =
	{
		50,50,50,
		20,20,20,
		2,2,2,
	};

float SceneElement::globalAmbient[] = { 0.4, 0.4, 0.4 };

SceneElement::SceneElement(ShaderIF* sIF) : shaderIF(sIF)
{
}

SceneElement::~SceneElement()
{
}

// NOTE: You may want to modify the interface to this method so that you
//       can pass parameters in case you want to establish lights differently
//       for different parts of the scene.
void SceneElement::establishLightingEnvironment( /* ... parameters? ... */ )
{
	// This should set all light source parameters, including:
	// "actualNumLights", "ecLightPosition", "lightStrength", "globalAmbient",
	// and whether the light is enabled.
	// When setting "ecLightPosition", be sure to transform the coordinates
	// from MC to EC using the current mc_ec matrix if the position was specified
	// in MC.
	glUniform1i(shaderIF->ppuLoc("actualNumLights"), MAX_NUM_LIGHTS);
	cryph::Matrix4x4 mc_ec, ec_lds;
	getMatrices(mc_ec, ec_lds);
	for(int i=0;i<MAX_NUM_LIGHTS;i++){
		int lightNum = i * 4;
		if(posInModelCoordinates[i]){
			if(lightPos[i * 4 + 3])
				(mc_ec * cryph::AffPoint(lightPos[lightNum], lightPos[lightNum + 1], lightPos[lightNum + 2])).aCoords(posToGLSL, lightNum);
			else
				(mc_ec.subMatrix(3, 3) * cryph::AffPoint(lightPos[lightNum], lightPos[lightNum + 1], lightPos[lightNum + 2])).aCoords(posToGLSL, lightNum);
			posToGLSL[lightNum + 3] = lightPos[lightNum + 3];
		}
		else{
			for(int j=0;j<4;j++){
				posToGLSL[lightNum + j] = lightPos[lightNum + j];
			}
		}
	}
	glUniform4fv(shaderIF->ppuLoc("ecLightPosition"), MAX_NUM_LIGHTS, posToGLSL);
	glUniform3fv(shaderIF->ppuLoc("lightStrength"), MAX_NUM_LIGHTS, lightStrength);
	// glUniform1iv(shaderIF->ppuLoc("lightEnabled"), MAX_NUM_LIGHTS, enabled);
	glUniform3fv(shaderIF->ppuLoc("globalAmbient"), 1, globalAmbient);
}

void SceneElement::establishMaterial(const PhongMaterial& matl)
{
	// This should set all material property parameters, including:
	// "ka", "kd", "ks", "m", and (for project 4) "alpha"
	glUniform3fv(shaderIF->ppuLoc("ka"), 1, matl.ka);
	glUniform3fv(shaderIF->ppuLoc("kd"), 1, matl.kd);
	glUniform3fv(shaderIF->ppuLoc("ks"), 1, matl.ks);
	glUniform1f(shaderIF->ppuLoc("m"), matl.m);
	glUniform1f(shaderIF->ppuLoc("alpha"), matl.alpha);
}

float SceneElement::establishSeason(){
	return 1.0;
}

// NOTE: You may want to modify the interface to this method so that you
//       can pass parameters to help you specify some of the parameters
//       mentioned in the comments that follow.
void SceneElement::establishTexture(GLuint texID, bool useAlpha,int mode)
{
	// Set texture-related parameters:
	// 1. OpenGL ones including glActiveTexture, glBindTexture, glTexParameter*
	// 2. Parameters communicated via per-primitive uniform variables defined in
	//    your shader program that allow you to control whether a texture is to
	//    to be used, and, if so, where the texture comes from (e.g., procedural,
	//    a sampler2D, etc.) and how to combine its color with with the color
	//    computed by your Phong local lighting model color.
	//
	// (If you are texture-mapping onto faces of BasicShape instances,
	// see that documentation for specific additional details.)
	glUniform1i(shaderIF->ppuLoc("textureImage"), 0);
	glUniform1i(shaderIF->ppuLoc("snowMap"), 1);
	useTexture = true;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texID);
}

void SceneElement::establishView(cryph::Matrix4x4 M)
{
	// Line of sight, dynamic view controls, 3D-2D projection, & mapping to LDS:
	cryph::Matrix4x4 mc_ec, ec_lds;
	ModelView::getMatrices(mc_ec, ec_lds);
	float m[16];
	glUniformMatrix4fv(shaderIF->ppuLoc("mc_ec"), 1, false, (mc_ec * M).extractColMajor(m));
	glUniformMatrix4fv(shaderIF->ppuLoc("og_mc_ec"), 1, false, mc_ec.extractColMajor(m));
	glUniformMatrix4fv(shaderIF->ppuLoc("ec_lds"), 1, false, ec_lds.extractColMajor(m));
	ExtendedController* ec = dynamic_cast<ExtendedController*>(Controller::getCurrentController());
	glUniform1i(shaderIF->ppuLoc("sceneHasTranslucentObjects"), 0);
	if (ec->drawingOpaque())
		glUniform1i(shaderIF->ppuLoc("drawingOpaqueObjects"), 1);
	else
		glUniform1i(shaderIF->ppuLoc("drawingOpaqueObjects"), 0);
}

GLuint SceneElement::readTextureImage(const std::string& imgFileName)
{
	// OpenGLImageReader* oglIR = OpenGLImageReader::create(imgFileName.c_str());
	// // if (oglIR == nullptr)
	// // {
	// // 	std::cerr << "Could not open '" << imgFileName << "' for texture map.\n";
	// // 	return 0;
	// // }
	// glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// GLuint texID[1];
	// glGenTextures(1, texID);
	// glBindTexture(GL_TEXTURE_2D, texID[0]);

	// // You may want to do one or more of the following in establishTexture
	// // rather than here.
	// float white[] = { 1.0, 1.0, 1.0, 1.0 };
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, white);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// // END: "You may want to ..."

	// GLint level = 0;
	// // int pw = oglIR->getWidth(), ph = oglIR->getHeight();
	// GLint iFormat = oglIR->getInternalFormat();
	// GLenum format = oglIR->getFormat();
	// GLenum type = oglIR->getType();
	// const GLint border = 0; // must be zero (only present for backwards compatibility)
	// const void* pixelData = oglIR->getTexture();
	// glTexImage2D(GL_TEXTURE_2D, level, iFormat, pw, ph, border, format, type, pixelData);
	// delete oglIR;
	return 0;
}

bool SceneElement::handleCommand(unsigned char anASCIIChar, double ldsX, double ldsY)
{
	switch (tolower(anASCIIChar))
	{
	case 'w':
        total_time = 62.8f;
		break;
    case 's':
		total_time = 0.0f;
		break;
    case ' ':
        paused = !paused;
		break;	
	case 'o':
		setProjection(ORTHOGONAL);
		break;
	case 'p':
		setProjection(PERSPECTIVE);
		break;
	case 'q':
		setProjection(OBLIQUE);
		break;
	default:
		break;
	}
	Controller::getCurrentController()->redraw();
	return true;
}

bool SceneElement::handleUpdate(void* update){
	return true;
}
