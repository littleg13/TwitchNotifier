#ifndef CHICKEN_H
#define CHICKEN_H

#include "MeshParticle.h"
#include "objLoader.h"

#define fileName "models/Chicken.obj"

class Chicken : public MeshParticle {
public:
    Chicken(ShaderIF* sIF, vec3 color);
    ~Chicken();
	void renderMeshParticle(cryph::Matrix4x4 M);

    static int getNVertices(){return n_vertices;}
    static int n_vertices;
    static int loadVertices(std::vector<std::array<double, 4>>* vertexArray, int p_offset);
    static int vertexOffset;
protected:
    void initMeshParticle();
    static GLuint ebo[1];
    static std::vector<int>* indices;
};
#endif