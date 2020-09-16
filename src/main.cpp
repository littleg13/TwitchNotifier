

#include "ExtendedController.h"
#include "Inline.h"
#include "SceneElements/ParticleSystem.h"
#include "PhongMaterial.h"

void createScene(ExtendedController& c, ShaderIF* particlesIF, ShaderIF* particleUpdatesIF, ShaderIF* physicsUpdatesIF)
{
	c.addModel(new ParticleSystem(particlesIF, particleUpdatesIF, physicsUpdatesIF, c.followers->size(), c.followers));
}

void set3DViewingInformation(double overallBB[])
{
	// IF we are using Viewing Strategy #1, THEN
	//     Notify class ModelView that we initially want to see the entire scene:
	//   ModelView::setMCRegionOfInterest(overallBB);
	ModelView::setMCRegionOfInterest(overallBB);
	// ELSE (Viewing Strategy #2)
	//     Modify all three of deltaX, deltaY, and deltaZ of the incoming overallBB
	//     to have the dimensions of the desired field of view, then invoke:
	//   ModelView::setMCRegionOfInterest(modified_overallBB);
	//   Tell the ModelView class that dynamic rotations are to be done about the eye.

	// MC -> EC:

	// TODO: Compute/set eye, center, up
	cryph::AffPoint eye, center;
	cryph::AffVector up;
	float center_arr[3];
	double r, d = 0;
	for(int i=0;i<6;i+=2){
		center_arr[i/2] = (overallBB[i+1] + overallBB[i]) / 2;
		r = cryph::maximum((overallBB[i+1] - overallBB[i]) / 2, r);
	}
	eye = cryph::AffPoint(0, 0, 1);
	up = cryph::AffPoint(0, 1, 0);
	center = cryph::AffPoint(center_arr);
	d = 9 * r;
	eye = center + d * eye;
	ModelView::setEyeCenterUp(eye, center, up);

	// EC -> LDS:

	// Specify the initial projection type desired
	ModelView::setProjection(PERSPECTIVE);

	// TODO: Compute/set ecZmin, ecZmax (It is often useful to exaggerate
	//       these limits somewhat to prevent unwanted depth clipping.)
	double ecZmin, ecZmax;
	// 1.5 is slightly larger than 2/sqrt(2). 2/sqrt(2) * r is the max distance to a corner of square.
	ecZmin = -d - 1.5 * r;
	ecZmax = -d + 1.5 * r;
	ModelView::setECZminZmax(ecZmin, ecZmax);

	// TODO: Compute/set ecZpp
	double ecZpp;
	ecZpp = ecZmax;
	ModelView::setProjectionPlaneZ(ecZpp);
}

int main(int argc, char* argv[])
{
	CoInitialize(NULL);
	ExtendedController c("Twitch Notifier", MVC_USE_DEPTH_BIT);
	c.reportVersions(std::cout);
	c.enableTwitch();

	ShaderIF::ShaderSpec vertexShader = {"shaders/basicParticle.vsh", GL_VERTEX_SHADER};
	ShaderIF::ShaderSpec fragmentShader = {"shaders/velColor_lit.fsh", GL_FRAGMENT_SHADER};
	ShaderIF::ShaderSpec geometryShader = {"shaders/addNormal.gsh", GL_GEOMETRY_SHADER};
	ShaderIF::ShaderSpec* specs = new ShaderIF::ShaderSpec[3];
	specs[0] = vertexShader;
	specs[1] = fragmentShader;
	specs[2] = geometryShader;
	ShaderIF* particlesIF = new ShaderIF(specs, 3);	
	ShaderIF::ShaderSpec particleUpdate = {"shaders/particleUpdate.comp", GL_COMPUTE_SHADER};
	ShaderIF::ShaderSpec physicsUpdate = {"shaders/physicsUpdate.comp", GL_COMPUTE_SHADER};
	
	ShaderIF::ShaderSpec* particleUpdateShaders = new ShaderIF::ShaderSpec[1];
	particleUpdateShaders[0] = particleUpdate;
	ShaderIF::ShaderSpec* physicsUpdateShaders = new ShaderIF::ShaderSpec[1];
	physicsUpdateShaders[0] = physicsUpdate;

	ShaderIF* particleUpdatesIF = new ShaderIF(particleUpdateShaders, 1);
	
	ShaderIF* physicsUpdatesIF = new ShaderIF(physicsUpdateShaders, 1);

	createScene(c, particlesIF, particleUpdatesIF, physicsUpdatesIF);

	double xyz[6];
	c.getOverallMCBoundingBox(xyz);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0,0,0,1);
	std::cout << "Bounding box: " << xyz[0] << " <= x <= " << xyz[1] << '\n';
	std::cout << "              " << xyz[2] << " <= y <= " << xyz[3] << '\n';
	std::cout << "              " << xyz[4] << " <= z <= " << xyz[5] << "\n\n";
	std::cout << "Controls:\nSpacebar - Pause\nW - Full Winter\nS - Full Summer\n\n";
	set3DViewingInformation(xyz);
	c.setRunWaitsForAnEvent(false);
	c.run();

	c.removeAllModels(true);

	return 0;
}
