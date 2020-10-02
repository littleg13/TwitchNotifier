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
		renderElements[follower->index]->setColor(follower->color);
	}
	else if(event->action == updateEvent::ACTION::CHANGE_SHAPE){
		Follower* follower = (*followers)[(*(event->info))["user"].string_value()];
		changeShape(follower->index, follower->shape);
		renderElements[follower->index]->setColor(follower->color);
	}
	return false;
}

void ParticleSystem::changeShape(int index, std::string shape){
	delete renderElements[index];
	float color[3] = {1.0, 1.0, 1.0};
	renderElements[index] = makeMesh(shape, shaderIF, color);
}

MeshParticle* ParticleSystem::makeMesh(std::string shape, ShaderIF* sIF, vec3 color){
	if(shape == "sphere"){
		return new Sphere(sIF, color);
	}
	else if(shape == "cube"){
		return new Cube(sIF, color);
	}
	else if(shape == "amongus"){
		return new AmongUs(sIF, color);
	}
	else if(shape == "chicken"){
		return new Chicken(sIF, color);
	}
	return new Cube(sIF, color);
}

void ParticleSystem::bufferVertexData(){
	vec4* vertexArray = new vec4[vertices->size()];
	for(int i=0;i<vertices->size();i++)
		for(int j=0;j<4;j++)
			vertexArray[i][j] = vertices->at(i)[j];
	int numBytesInBuffer = vertices->size() * sizeof(vec4);
	glBufferData(GL_ARRAY_BUFFER, numBytesInBuffer, vertexArray, GL_STATIC_DRAW);
}

void ParticleSystem::loadMeshes(std::vector<std::array<double, 4>>* vertexArray){
	int offset = 0;
	offset += Cube::loadVertices(vertexArray, offset);
	offset += Sphere::loadVertices(vertexArray, offset);
	offset += AmongUs::loadVertices(vertexArray, offset);
	offset += Chicken::loadVertices(vertexArray, offset);
}

void ParticleSystem::initParticles(){
	n_particles = MAX_PARTICLES;
	particles = new Particle[n_particles];
	positions = new vec3[n_particles];
	vertices = new std::vector<std::array<double, 4>>;
	renderElements = new MeshParticle*[n_particles];
	loadMeshes(vertices);
	std::string* shapes = new std::string[n_particles];
	for(auto it = followers->begin(); it != followers->end(); ++it){
		shapes[it->second->index] = it->second->shape;
	}
	for(int i=0;i<n_particles;i++){

		particles[i].position[0] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 5 - 2.5;
		particles[i].position[1] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 5 - 2.5;
		particles[i].position[2] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 5 - 2.5;
		particles[i].position[3] = 1.0;
		float particleSize = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.3 + 0.25;
		particles[i].scale[0] = particleSize;
		particles[i].scale[1] = particleSize;
		particles[i].scale[2] = particleSize;
		particles[i].scale[3] = 1.0;
		vec3 color = {
			(static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.1 + 0.1,
			(static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.3 + 0.1,
			(static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.4 + 0.4,
		};
		renderElements[i] = makeMesh(shapes[i], shaderIF, color);
	
		particles[i].frameUpdated = 0;
		

		particles[i].origin[0] = particles[i].position[0];
		particles[i].origin[1] = particles[i].position[1];
		particles[i].origin[2] = particles[i].position[2];
		particles[i].origin[3] = particles[i].position[3];

        particles[i].transformation[0] = particles[i].position[0];
        particles[i].transformation[1] = particles[i].position[1];
        particles[i].transformation[2] = particles[i].position[2];
		particles[i].transformation[3] = particles[i].position[3];
		particles[i].velocity[0] = 0;// * 0.8 - 0.4;
		particles[i].velocity[1] = 0;//(static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.8 - 0.4;
		particles[i].velocity[2] = 0;//(static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * 0.8 - 0.4;
		particles[i].velocity[3] = 1.0;
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
			renderElements[it->second->index]->setColor(it->second->color);
		}
	}

	glGenVertexArrays(1, vao);
	glGenBuffers(2, vbo);

	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	MeshParticle::setVAO(vao[0]);
	int numBytesInBuffer = n_particles * sizeof(Particle);
	glBufferData(GL_ARRAY_BUFFER, numBytesInBuffer, particles, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	MeshParticle::setVBO(vbo[1]);
	bufferVertexData();
	glVertexAttribPointer(shaderIF->pvaLoc("mcPosition"), 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderIF->pvaLoc("mcPosition"));


	glPointSize(1.0);


	glUseProgram(shaderIF->getShaderPgmID());
	
}

void ParticleSystem::renderParticleSystem(cryph::Matrix4x4 M){
	Particle particle;
	for(int i=0;i<n_active_particles;i++){
		glGetNamedBufferSubData(vbo[0], i*sizeof(Particle), sizeof(Particle), &particle);
		float mat[16];
		cryph::Matrix4x4 rotMat = cryph::Matrix4x4::yRotationRadians(100 * particle.frameUpdated * ((particle.seed * 2) - 1));
		cryph::Matrix4x4 transMat = cryph::Matrix4x4::translation(cryph::AffVector(particle.transformation));
		cryph::Matrix4x4 scaleMat = cryph::Matrix4x4::scale(particle.scale[0], particle.scale[1], particle.scale[2]);
		renderElements[i]->render(M * transMat * rotMat * scaleMat);
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
	particlePass();
	// restore the previous program
	glUseProgram(pgm);
}

