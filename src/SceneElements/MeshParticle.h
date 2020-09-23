
#ifndef MESHPARTICLE_H
#define MESHPARTICLE_H

#include <array>

#include "SceneElement.h"
#include "GLFWController.h"

typedef float vec4[4];
typedef float vec3[3];

class MeshParticle : public SceneElement{
public:
    MeshParticle(ShaderIF* sIF, vec3 color);
	virtual ~MeshParticle();
    void setColor(float color[3]);
	void getMCBoundingBox(double* xyzLimits) const;
	bool handleCommand(unsigned char anASCIIChar, double ldsX, double ldsY);
	void render();
    void render(cryph::Matrix4x4 M) {renderMeshParticle(M);}
	virtual void renderMeshParticle(cryph::Matrix4x4 M){};
    static void setVAO(GLuint param){vao = param;};
    static void setVBO(GLuint param){vbo = param;};
    virtual int getNVertices() {return 0;}
protected:
    virtual void initMeshParticle();
    static GLuint vao;
    static GLuint vbo;
	PhongMaterial phongMat;

};
#endif