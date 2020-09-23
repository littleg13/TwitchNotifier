
#include <iostream>
#include "cube.h"

int Cube::n_vertices = 8;
int Cube::vertexOffset = 0;

GLuint Cube::ebo[6];

Cube::Cube(ShaderIF* sIF, vec3 color) : MeshParticle(sIF, color){
};

Cube::~Cube(){
}

int Cube::loadVertices(std::vector<std::array<double, 4>>* vertexArray, int p_offset){
    std::array<double, 4> pArr;
    for(int i=0;i<8;i++){
        for(int j=0;j<3;j++){
			pArr[j] = (float((i >> j) & 1) - 0.5);
        }
		pArr[3] = 1.0;
        vertexArray->push_back(pArr);
    }
    vertexOffset = p_offset;
    int indices[6][4] = {
        {0, 1, 2, 3},
        {1, 5, 3, 7},
        {5, 4, 7, 6},
        {4, 0, 6, 2},
        {2, 3, 6, 7},
        {0, 1, 4, 5}
    };
    for(int i=0;i<6;i++)
        for(int j=0;j<4;j++)
            indices[i][j] += vertexOffset;
    glBindVertexArray(vao);
    glGenBuffers(6, ebo);
    for(int i=0;i<6;i++){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4*sizeof(GLuint), indices[i], GL_STATIC_DRAW);
    }
    return getNVertices();
}

void Cube::initMeshParticle(){
    
}

void Cube::renderMeshParticle(cryph::Matrix4x4 M){
	// draw the triangles using our vertex and fragment shaders
	glUseProgram(shaderIF->getShaderPgmID());

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

    float mat[16];
	glUniformMatrix4fv(shaderIF->ppuLoc("mc_ec"), 1, false, M.extractColMajor(mat));
	establishMaterial(phongMat);
	establishLightingEnvironment();
    for(int i=0;i<6;i++){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[i]);
	    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, nullptr);
    }
}
