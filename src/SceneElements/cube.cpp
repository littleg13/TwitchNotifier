
#include <iostream>
#include "cube.h"


GLuint Cube::vao = 0;
GLuint Cube::vbo = 0;

Cube::Cube(ShaderIF* sIF, vec4* vertexArray, int offset, vec3 pos, vec3 size, vec3 color) : SceneElement(sIF)
{
	solidSnow = PhongMaterial(color[0], color[1], color[2], 0.2, 0.8, 0.2, 2, 1);
	initCube(vertexArray, offset, pos, size);
}

Cube::~Cube()
{
    glDeleteBuffers(6, ebo);
	
}

void Cube::getMCBoundingBox(double* xyzLimits) const
{
	xyzLimits[0] = -5; // xmin  Give real values!
	xyzLimits[1] = 5; // xmax         |
	xyzLimits[2] = -5; // ymin         |
	xyzLimits[3] = 5; // ymax         |
	xyzLimits[4] = -5; // zmin         |
	xyzLimits[5] = 5; // zmax        \_/
}

bool Cube::handleCommand(unsigned char anASCIIChar, double ldsX, double ldsY)
{
	return true;
}

void Cube::initCube(vec4* vertexArray, int offset, vec3 pos, vec3 size){
    for(int i=0;i<8;i++){
        for(int j=0;j<3;j++){
            vertexArray[i + offset][j] = pos[j] + (float((i >> j) & 1) - 0.5) *  size[j];
        }
        vertexArray[i + offset][3] = 1.0;
    }
    for(int i=0;i<6;i++)
        for(int j=0;j<4;j++)
            indices[i][j] += offset;
    glBindVertexArray(vao);
    glGenBuffers(6, ebo);
    for(int i=0;i<6;i++){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4*sizeof(GLuint), indices[i], GL_STATIC_DRAW);
    }
    
}

void Cube::setColor(float color[3]){
	solidSnow = PhongMaterial(color[0], color[1], color[2], 0.2, 0.8, 0.2, 2, 1);
}

void Cube::renderCube(cryph::Matrix4x4 M){
	// draw the triangles using our vertex and fragment shaders
	glUseProgram(shaderIF->getShaderPgmID());

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

    float mat[16];
	glUniformMatrix4fv(shaderIF->ppuLoc("mc_ec"), 1, false, M.extractColMajor(mat));
	establishMaterial(solidSnow);
	establishLightingEnvironment();
	float fastColor[3] = {
		0.1, 0.3, 0.9
	};
    for(int i=0;i<6;i++){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[i]);
	    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, nullptr);
    }
}


void Cube::render()
{
	// save the current GLSL program in use
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);
    cryph::Matrix4x4 mc_ec, ec_lds;
	getMatrices(mc_ec, ec_lds);
	float mat[16];
	glUniformMatrix4fv(shaderIF->ppuLoc("ec_lds"), 1, false, ec_lds.extractColMajor(mat));
    
	renderCube(mc_ec);
	glUseProgram(pgm);
}