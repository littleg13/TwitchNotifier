// ParticleSystem.h

#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "SceneElement.h"
#include "GLFWController.h"
#include "cube.h"

#define VERTICES_PER_PARTICLE 8
#define MAX_PARTICLES 5000

typedef float vec3[3];
typedef float vec4[4];
typedef float vec2[2];
typedef struct Particle
{
	vec4 position;
	int vertexIndices[VERTICES_PER_PARTICLE];
	vec4 velocity;
	vec4 quaternion;
	vec4 rotAxis;
	vec4 origin;
	float seed;
	float frameUpdated;
	int padding[2];
} Particle;


class ParticleSystem : public SceneElement
{
	

public:
	ParticleSystem(ShaderIF* sIF, ShaderIF* particleUpdatesIF, ShaderIF* physicsUpdatesIF, int n_par, followerDict* p_followers);
	virtual ~ParticleSystem();
	void getMCBoundingBox(double* xyzLimits) const;
	bool handleCommand(unsigned char anASCIIChar, double ldsX, double ldsY);
	void render();
	void renderParticleSystem(cryph::Matrix4x4 M);
	bool handleUpdate(void* update);
private:
	int n_particles;// hash_size;
	int n_active_particles;
	int current_frame = 0;
	//Cell spacing
	float d = 1;
	Particle* particles;
	vec3* positions;
	vec4* vertices;
	SceneElement** renderElements;
	followerDict* followers;
	// uint32_t* hash_table;
	GLuint vao[1];
	GLuint vbo[2];
	ShaderIF* particleUpdatesIF;
	ShaderIF* physicsUpdatesIF;
	void particlePass();
	void physicsPass();
	void initParticles();
	PhongMaterial solidSnow = PhongMaterial(1.0, 1.0, 1.0, 0.2, 0.9, 0.2, 5, 1);
	float last_time = -1;
	bool paused = false;
	float total_time = 0;
};

#endif
