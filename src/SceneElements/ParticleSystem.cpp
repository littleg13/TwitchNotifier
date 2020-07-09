// ParticleSystem.c++ - a basic combined Model and View for OpenGL

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <math.h>

#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(ShaderIF* sIF, ShaderIF* particleUpdatesIF, ShaderIF* physicsUpdatesIF, int n_par, followerDict* p_followers) : SceneElement(sIF), particleUpdatesIF(particleUpdatesIF), physicsUpdatesIF(physicsUpdatesIF), n_active_particles(n_par), followers(p_followers)
{
	initParticles();
}

ParticleSystem::~ParticleSystem()
{
    glDeleteBuffers(2, vbo);
    glDeleteVertexArrays(1, vao);
	delete[] particles;
	delete[] positions;
	// delete[] hash_table;
	
}

void ParticleSystem::getMCBoundingBox(double* xyzLimits) const
{
	xyzLimits[0] = -5; // xmin  Give real values!
	xyzLimits[1] = 5; // xmax         |
	xyzLimits[2] = -5; // ymin         |
	xyzLimits[3] = 5; // ymax         |
	xyzLimits[4] = -5; // zmin         |
	xyzLimits[5] = 5; // zmax        \_/
}

bool ParticleSystem::handleCommand(unsigned char anASCIIChar, double ldsX, double ldsY)
{
	return true;
}

bool ParticleSystem::handleUpdate(void* update){
	updateEvent* event = reinterpret_cast<updateEvent*>(update);
	if(event->action == updateEvent::ACTION::NEW_FOLLOWER){
		n_active_particles = followers->size();
	}
	else if(event->action == updateEvent::ACTION::CHANGE_COLOR){
		Follower* follower = (*followers)[(*(event->info))["user"].string_value()];
		(reinterpret_cast<Cube*>(renderElements[follower->index]))->setColor(follower->color);
	}
	return false;
}

void ParticleSystem::initParticles(){
	n_particles = MAX_PARTICLES;
	particles = new Particle[n_particles];
	positions = new vec3[n_particles];
	vertices = new vec4[n_particles * VERTICES_PER_PARTICLE];
	renderElements = new SceneElement*[n_particles];

	for(int i=0;i<n_particles;i++){

		particles[i].position[0] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 5 - 2.5;
		particles[i].position[1] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 5 - 2.5;
		particles[i].position[2] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 5 - 2.5;
		particles[i].position[3] = 1.0;
		float cubeSize = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.3 + 0.25;
		vec3 size = {cubeSize, cubeSize, cubeSize};
		vec3 color = {
			(static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.1 + 0.1,
			(static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.3 + 0.1,
			(static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.4 + 0.4,
		};
		renderElements[i] = new Cube(shaderIF, vertices, i * VERTICES_PER_PARTICLE, particles[i].position, size, color);
		for(int j=0;j<VERTICES_PER_PARTICLE;j++){
			particles[i].vertexIndices[j] = i * VERTICES_PER_PARTICLE + j;
		}
		particles[i].frameUpdated = 0;
		

		particles[i].origin[0] = particles[i].position[0];
		particles[i].origin[1] = particles[i].position[1];
		particles[i].origin[2] = particles[i].position[2];
		particles[i].origin[3] = particles[i].position[3];

        particles[i].quaternion[0] = 0;
        particles[i].quaternion[1] = 0;
        particles[i].quaternion[2] = 0;
		particles[i].quaternion[3] = 1;
		particles[i].velocity[0] = 0;// * 0.8 - 0.4;
		particles[i].velocity[1] = 0;//(static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.8 - 0.4;
		particles[i].velocity[2] = 0;//(static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.8 - 0.4;
		particles[i].velocity[3] = 0;
		particles[i].rotAxis[0] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2 - 1;
		particles[i].rotAxis[1] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2 - 1;
		particles[i].rotAxis[2] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 2 - 1;
		particles[i].seed = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
		cryph::AffVector randomVector = cryph::AffVector(particles[i].rotAxis);
		cryph::AffVector posVector = cryph::AffVector(particles[i].position);
		cryph::AffVector orbitDirection = randomVector.cross(posVector);
		orbitDirection.normalize();
		randomVector = orbitDirection.cross(posVector);
		randomVector.vComponents(particles[i].rotAxis);
	}
	for(auto it = followers->begin(); it != followers->end(); ++it){
		if(it->second->hasColor){
			(reinterpret_cast<Cube*>(renderElements[it->second->index]))->setColor(it->second->color);
		}
	}




	glGenVertexArrays(1, vao);
	glGenBuffers(2, vbo);

	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	Cube::setVAO(vao[0]);
	int numBytesInBuffer = n_particles * sizeof(Particle);
	glBufferData(GL_ARRAY_BUFFER, numBytesInBuffer, particles, GL_STATIC_DRAW);
	// glVertexAttribPointer(shaderIF->pvaLoc("mcVelocity"), 3, GL_FLOAT, GL_FALSE, sizeof(Particle), reinterpret_cast<void*>(0 + offsetof(Particle, velocity)));
	// glEnableVertexAttribArray(shaderIF->pvaLoc("mcVelocity"));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	Cube::setVBO(vbo[1]);
	numBytesInBuffer = n_particles * VERTICES_PER_PARTICLE * sizeof(vec4);
	glBufferData(GL_ARRAY_BUFFER, numBytesInBuffer, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(shaderIF->pvaLoc("mcPosition"), 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderIF->pvaLoc("mcPosition"));

	// glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	// numBytesInBuffer = hash_size * sizeof(uint32_t);
	// glBufferData(GL_ARRAY_BUFFER, numBytesInBuffer, hash_table, GL_STATIC_DRAW);
	glPointSize(1.0);


	glUseProgram(shaderIF->getShaderPgmID());
	
}

void quatToMatrix(vec4 q, cryph::Matrix4x4& rotMat){
	float x = q[0];
	float y = q[1];
	float z = q[2];
	float w = q[3];
	cryph::Matrix3x3 m(
		1 - 2*y*y - 2*z*z, 2*x*y - 2*w*z, 2*x*z + 2*w*y,
		2*x*y + 2*w*z, 1 - 2*x*x - 2*z*z, 2*y*z - 2*w*x,
		2*x*z - 2*w*y, 2*y*z + 2*w*x, 1 - 2*x*x - 2*y*y
	);
	rotMat = cryph::Matrix4x4(m);
}

void ParticleSystem::renderParticleSystem(cryph::Matrix4x4 M){
	Particle particle;
	for(int i=0;i<n_active_particles;i++){
		glGetNamedBufferSubData(vbo[0], i*sizeof(Particle), sizeof(Particle), &particle);
		cryph::Matrix4x4 rotMat;
		quatToMatrix(particle.quaternion, rotMat);
		cryph::AffVector vel = cryph::AffVector(particle.velocity);
		vel.normalize();
		cryph::AffPoint origin = cryph::AffPoint(particle.position);
		rotMat = cryph::Matrix4x4::yRotationRadians(100 * particle.frameUpdated * ((particle.seed * 2) - 1));
		float mat[16];
		glUniformMatrix4fv(shaderIF->ppuLoc("transformationMat"), 1, false, rotMat.extractColMajor(mat));
		glUniform3fv(shaderIF->ppuLoc("origin"), 1, particle.position);
		renderElements[i]->render(M);
	}
}

void ParticleSystem::particlePass(){
	glUseProgram(particleUpdatesIF->getShaderPgmID());

	if(last_time == -1)
		last_time = glfwGetTime();
    float delta_time = glfwGetTime() - last_time;
    if(!paused)
        total_time += delta_time;
	glUniform1f(particleUpdatesIF->ppuLoc("deltaTime"), glfwGetTime() - last_time);
	last_time = glfwGetTime();

	glUniform1i(particleUpdatesIF->ppuLoc("n_active_particles"), n_active_particles);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, vbo[0]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, vbo[1]);

	int warp_count = ceil((float)n_particles / (float)256);

	glDispatchCompute(warp_count, 1, 1);
	glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}

void ParticleSystem::physicsPass(){
	glUseProgram(physicsUpdatesIF->getShaderPgmID());

	glUniform1i(physicsUpdatesIF->ppuLoc("n_active_particles"), n_active_particles);
	
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, vbo[0]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, vbo[1]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, vbo[1]);

	int warp_count = ceil((float)n_particles / (float)256);

	glDispatchCompute(warp_count, 1, 1);
	glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

}


void ParticleSystem::render()
{
	// save the current GLSL program in use
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);
    cryph::Matrix4x4 mc_ec, ec_lds;
	getMatrices(mc_ec, ec_lds);
	float mat[16];
	glUniformMatrix4fv(shaderIF->ppuLoc("ec_lds"), 1, false, ec_lds.extractColMajor(mat));
	establishLightingEnvironment();
	renderParticleSystem(mc_ec);
	current_frame = (current_frame + 1) % 256;
	// physicsPass();
	particlePass();
	// restore the previous program
	glUseProgram(pgm);
}

