#include <stdio.h>
#include <iostream>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "raytracing.h"

#include <glm/gtc/type_ptr.hpp>

#include <traceur/core/kernel/basic.hpp>
#include <traceur/core/scene/graph/factory.hpp>
#include <traceur/core/scene/graph/vector.hpp>
#include <traceur/loader/obj.hpp>

#include <traceur/frontend/glut/visitor.hpp>

//temporary variables
//these are only used to illustrate 
//a simple debug drawing. A ray 
glm::vec3 testRayOrigin;
glm::vec3 testRayDestination;

// The rendering kernel to use
std::unique_ptr<traceur::BasicKernel> kernel;
// The scene we want to render
std::unique_ptr<traceur::Scene> scene;
// The scene graph visitor to draw the scene.
std::unique_ptr<traceur::SceneGraphVisitor> visitor;

//use this function for any preprocessing of the mesh.
void init()
{	
	//one first move: initialize the first light source
	//at least ONE light source has to be in the scene!!!
	//here, we set it to the current location of the camera
	MyLightPositions.push_back(MyCameraPosition);

	auto factory = traceur::make_factory<traceur::VectorSceneGraphBuilder>();
	auto loader = std::make_unique<traceur::ObjLoader>(std::move(factory));
	scene = loader->load("cube.obj");

	kernel = std::make_unique<traceur::BasicKernel>();
	visitor = std::make_unique<traceur::OpenGLSceneGraphVisitor>();
}

// Render the scene that has been loaded
std::unique_ptr<traceur::Film> render(const traceur::Camera &camera)
{
	return kernel->render(*scene, camera);
}

void yourDebugDraw()
{
	// Draw the loaded scene graph
	scene->graph->traverse(*visitor);

	
	//let's draw the lights in the scene as points
	glPushAttrib(GL_ALL_ATTRIB_BITS); //store all GL attributes
	glDisable(GL_LIGHTING);
	glColor3f(1,1,1);
	glPointSize(10);
	glBegin(GL_POINTS);
		for (int i = 0; i < MyLightPositions.size(); ++i)
			glVertex3fv(glm::value_ptr(MyLightPositions[i]));
	glEnd();
	glPopAttrib();//restore all GL attributes
	//The Attrib commands maintain the state. 
	//e.g., even though inside the two calls, we set
	//the color to white, it will be reset to the previous 
	//state after the pop.


	//as an example: we draw the test ray, which is set by the keyboard function
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
		glColor3f(0,1,1);
		glVertex3f(testRayOrigin[0], testRayOrigin[1], testRayOrigin[2]);
		glColor3f(0,0,1);
		glVertex3f(testRayDestination[0], testRayDestination[1], testRayDestination[2]);
	glEnd();
	glPointSize(10);
	glBegin(GL_POINTS);
		glVertex3fv(glm::value_ptr(MyLightPositions[0]));
	glEnd();
	glPopAttrib();
}


//yourKeyboardFunc is used to deal with keyboard input.
//t is the character that was pressed
//x,y is the mouse position in pixels
//rayOrigin, rayDestination is the ray that is going in the view direction UNDERNEATH your mouse position.
//
//A few keys are already reserved: 
//'L' adds a light positioned at the camera location to the MyLightPositions vector
//'l' modifies the last added light to the current 
//    camera position (by default, there is only one light, so move it with l)
//    ATTENTION These lights do NOT affect the real-time rendering. 
//    You should use them for the raytracing.
//'r' calls the function performRaytracing on EVERY pixel, using the correct associated ray. 
//    It then stores the result in an image "result.ppm".
//    Initially, this function is fast (performRaytracing simply returns 
//    the target of the ray - see the code above), but once you replaced 
//    this function and raytracing is in place, it might take a 
//    while to complete...
void yourKeyboardFunc(char t, int x, int y, const glm::vec3 &rayOrigin, const glm::vec3 &rayDestination)
{

	//here, as an example, I use the ray to fill in the values for my upper global ray variable
	//I use these variables in the debugDraw function to draw the corresponding ray.
	//try it: Press a key, move the camera, see the ray that was launched as a line.
	testRayOrigin = rayOrigin;
	testRayDestination = rayDestination;
}
