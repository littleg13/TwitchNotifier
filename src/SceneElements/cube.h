
#ifndef CUBE_H
#define CUBE_H

#include "SceneElement.h"
#include "GLFWController.h"

typedef float vec4[4];
typedef float vec3[3];

class Cube : public SceneElement{
    public:
    Cube(ShaderIF* sIF, vec4* vertexArray, int offset, vec3 pos, vec3 size, vec3 color);
	virtual ~Cube();
    void setColor(float color[3]);
	// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
	void getMCBoundingBox(double* xyzLimitsF) const;
	bool handleCommand(unsigned char anASCIIChar, double ldsX, double ldsY);
	void render();
    void render(cryph::Matrix4x4 M) {renderCube(M);}
	void renderCube(cryph::Matrix4x4 M);
    static void setVAO(GLuint param){vao = param;};
    static void setVBO(GLuint param){vbo = param;};
    private:
    void initCube(vec4* vertexArray, int offset, vec3 pos, vec3 size);
    static GLuint vao;
    static GLuint vbo;
	PhongMaterial solidSnow;

    int indices[6][4] = {
        {0, 1, 2, 3},
        {1, 5, 3, 7},
        {5, 4, 7, 6},
        {4, 0, 6, 2},
        {2, 3, 6, 7},
        {0, 1, 4, 5}
    };

    float normals[6][3] = {
        {0, 0, -1},
        {0, 0, 1},
        {-1, 0, 0},
        {1, 0, 0},
        {0, -1, 0},
        {0, 1, 0},
    };
    GLuint ebo[6];

    protected:


};
#endif