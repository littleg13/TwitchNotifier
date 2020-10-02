
#include <iostream>
#include "sphere.h"

int Sphere::stackCount = PRECISION;
int Sphere::sectorCount = PRECISION;
int Sphere::n_vertices = 0;
int Sphere::vertexOffset = 0;

GLuint Sphere::ebo[PRECISION];

Sphere::Sphere(ShaderIF* sIF, vec3 color) : MeshParticle(sIF, color){
}

Sphere::~Sphere(){
}

int Sphere::loadVertices(std::vector<std::array<double, 4>>* vertexArray, int p_offset){
    cryph::AffVector u = cryph::AffVector::xu;
    cryph::AffVector v = cryph::AffVector::yu;
    cryph::AffVector w = cryph::AffVector::zu;
    float dPhi = PI / float(stackCount - 1);
    float dTheta = (2.0 * PI) / float(sectorCount - 1);
    for(int i=0;i<stackCount;i++){
        float theta = i * dTheta;
        for(int j=0;j<sectorCount;j++){
            float phi = (PI / 2.0) - j * dPhi;
            cryph::AffVector nHat = cos(phi)*(cos(theta)*u + sin(theta)*v) + sin(phi)*w;
            cryph::AffPoint p = nHat * 0.6;
            std::array<double, 4> pArr;
            p.aCoords(pArr.data());
            pArr[3] = 1.0;
            vertexArray->push_back(pArr);
            n_vertices++;
        } 
    }
    vertexOffset = p_offset;
    std::vector<int> indices;
    for(int i=0;i<stackCount;i++){
        for(int j=0;j<sectorCount;j++){
            indices.push_back(vertexOffset + i*(sectorCount) + j);
            if(i == stackCount - 1)
                indices.push_back(vertexOffset + j);
            else
                indices.push_back(vertexOffset + (i+1)*(sectorCount) + j);
        }
    }
    glBindVertexArray(vao);
    glGenBuffers(stackCount, ebo);
    for(int i=0;i<stackCount;i++){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (indices.size() / stackCount) *sizeof(GLuint), &indices[(i*2)*sectorCount], GL_STATIC_DRAW);
    }
    return getNVertices();
}

void Sphere::initMeshParticle(){
}

void Sphere::renderMeshParticle(cryph::Matrix4x4 M){
	// draw the triangles using our vertex and fragment shaders
	glUseProgram(shaderIF->getShaderPgmID());

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

    float mat[16];
	glUniformMatrix4fv(shaderIF->ppuLoc("mc_ec"), 1, false, M.extractColMajor(mat));
	establishMaterial(phongMat);
	establishLightingEnvironment();
    for(int i=0;i<stackCount;i++){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo[i]);
	    glDrawElements(GL_TRIANGLE_STRIP, (2 * stackCount), GL_UNSIGNED_INT, nullptr);
    }
}
 