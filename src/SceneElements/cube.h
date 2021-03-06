
#ifndef CUBE_H
#define CUBE_H

#include "MeshParticle.h"

typedef float vec4[4];
typedef float vec3[3];

class Cube : public MeshParticle{
public:
    Cube(ShaderIF* sIF, vec3 color);
    ~Cube();
	void renderMeshParticle(cryph::Matrix4x4 M);

    static int getNVertices(){return n_vertices;}
    static int n_vertices;
    static int loadVertices(std::vector<std::array<double, 4>>* vertexArray, int p_offset);
    static int vertexOffset;
protected:
    void initMeshParticle();
    static GLuint ebo[6];
};
#endif