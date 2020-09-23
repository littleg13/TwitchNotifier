
#include <iostream>
#include "MeshParticle.h"


GLuint MeshParticle::vao = 0;
GLuint MeshParticle::vbo = 0;

MeshParticle::MeshParticle(ShaderIF* sIF, vec3 color) : SceneElement(sIF)
{
	phongMat = PhongMaterial(color[0], color[1], color[2], 0.2, 0.8, 0.2, 2, 1);
}

void MeshParticle::initMeshParticle(){};

MeshParticle::~MeshParticle()
{
}

void MeshParticle::getMCBoundingBox(double* xyzLimits) const
{
	xyzLimits[0] = -5; // xmin  Give real values!
	xyzLimits[1] = 5; // xmax         |
	xyzLimits[2] = -5; // ymin         |
	xyzLimits[3] = 5; // ymax         |
	xyzLimits[4] = -5; // zmin         |
	xyzLimits[5] = 5; // zmax        \_/
}

bool MeshParticle::handleCommand(unsigned char anASCIIChar, double ldsX, double ldsY)
{
	return true;
}

void MeshParticle::setColor(float color[3]){
	phongMat = PhongMaterial(color[0], color[1], color[2], 0.2, 0.8, 0.2, 2, 1);
}

void MeshParticle::render()
{
	// save the current GLSL program in use
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);
    cryph::Matrix4x4 mc_ec, ec_lds;
	getMatrices(mc_ec, ec_lds);
	float mat[16];
	glUniformMatrix4fv(shaderIF->ppuLoc("ec_lds"), 1, false, ec_lds.extractColMajor(mat));
    
	renderMeshParticle(mc_ec);
	glUseProgram(pgm);
}