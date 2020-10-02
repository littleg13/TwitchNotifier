#ifndef AMONGUS_H
#define AMONGUS_H

#include "MeshParticle.h"
#include "objLoader.h"

#define fileName "models/amongus.obj"

class AmongUs : public MeshParticle {
public:
    AmongUs(ShaderIF* sIF, vec3 color);
    ~AmongUs();
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