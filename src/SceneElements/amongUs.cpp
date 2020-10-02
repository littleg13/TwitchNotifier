
#include <iostream>
#include "amongUs.h"

int AmongUs::n_vertices = 0;
int AmongUs::vertexOffset = 0;
std::vector<int>* AmongUs::indices = new std::vector<int>;

GLuint AmongUs::ebo[1];

AmongUs::AmongUs(ShaderIF* sIF, vec3 color) : MeshParticle(sIF, color){
};

AmongUs::~AmongUs(){
}

int AmongUs::loadVertices(std::vector<std::array<double, 4>>* vertexArray, int p_offset){
    vertexOffset = p_offset;
    std::vector<std::array<double, 4>>* normals = new std::vector<std::array<double, 4>>;
    std::vector<std::array<double, 2>>* texCoords = new std::vector<std::array<double, 2>>;
    n_vertices = ObjLoader::loadObjFile(fileName, vertexArray, indices, normals, texCoords, vertexOffset, 0.005);
    glBindVertexArray(vao);
    glGenBuffers(1, ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size()*sizeof(GLuint), indices->data(), GL_STATIC_DRAW);
    return AmongUs::getNVertices();
}

void AmongUs::initMeshParticle(){
    
}

void AmongUs::renderMeshParticle(cryph::Matrix4x4 M){
	// draw the triangles using our vertex and fragment shaders
	glUseProgram(shaderIF->getShaderPgmID());

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

    float mat[16];
	glUniformMatrix4fv(shaderIF->ppuLoc("mc_ec"), 1, false, M.extractColMajor(mat));
	establishMaterial(phongMat);
	establishLightingEnvironment();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[0]);
    glDrawElements(GL_TRIANGLES, indices->size(), GL_UNSIGNED_INT, nullptr);
}
