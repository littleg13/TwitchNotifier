
#ifndef SPHERE_H
#define SPHERE_H

#include "MeshParticle.h"
#define PI 3.1415
#define PRECISION 50

typedef float vec4[4];
typedef float vec3[3];



class Sphere : public MeshParticle{
public:
    Sphere(ShaderIF* sIF, vec3 color);
    ~Sphere();
	void renderMeshParticle(cryph::Matrix4x4 M);
    static int getNVertices(){return n_vertices;}
    static int n_vertices;
    static int loadVertices(std::vector<std::array<double, 4>>* vertexArray, int p_offset);
    static int vertexOffset;

protected:
    void initMeshParticle();
    static int stackCount;
    static int sectorCount;
    static GLuint ebo[PRECISION];
};
#endif