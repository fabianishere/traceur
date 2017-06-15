#include <stdio.h>
#include <limits>
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "raytracing.h"


//temporary variables
//these are only used to illustrate 
//a simple debug drawing. A ray 
Vec3Df testRayOrigin;
Vec3Df testRayDestination;


//use this function for any preprocessing of the mesh.
void init()
{
	//load the mesh file
	//please realize that not all OBJ files will successfully load.
	//Nonetheless, if they come from Blender, they should, if they 
	//are exported as WavefrontOBJ.
	//PLEASE ADAPT THE LINE BELOW TO THE FULL PATH OF THE dodgeColorTest.obj
	//model, e.g., "C:/temp/myData/GraphicsIsFun/dodgeColorTest.obj", 
	//otherwise the application will not load properly
    MyMesh.loadMesh("cube.obj", true);
	MyMesh.computeVertexNormals();

	//one first move: initialize the first light source
	//at least ONE light source has to be in the scene!!!
	//here, we set it to the current location of the camera
	MyLightPositions.push_back(MyCameraPosition);
}

inline double intersection(const Vec3Df &origin, const Vec3Df &dir, const Triangle &triangle)
{
	auto infinity = std::numeric_limits<double>::infinity();
	auto &vertices = MyMesh.vertices;
	auto o = vertices[triangle.v[0]].p;
	auto u = vertices[triangle.v[1]].p - o;
	auto v = vertices[triangle.v[2]].p - o;


	// Compute the plane's normal
	auto N = Vec3Df::crossProduct(u, v);

	// Calculate angle of ray relative to plane's normal
	auto d = Vec3Df::dotProduct(N, dir);

	// No intersection if the ray is parallel to plane
	if (std::abs(d) < 0.000000f) {
		return infinity;
	}

	// Solve t for equation P = O + tD
	double t = Vec3Df::dotProduct(o - origin, N) / d;

	// The triangle is behind the ray
	if (t < 0) {
		return infinity;
	}

	// Calculate the point of intersection
	auto p = origin + t * dir;

	float d00 = Vec3Df::dotProduct(u, u);
	float d01 = Vec3Df::dotProduct(u, v);
	float d11 = Vec3Df::dotProduct(v, v);
	float d20 = Vec3Df::dotProduct(p - o, u);
	float d21 = Vec3Df::dotProduct(p - o, v);
	float invDenom = 1.0 / (d00 * d11 - d01 * d01);

	auto a = (d11 * d20 - d01 * d21) * invDenom;
	auto b = (d00 * d21 - d01 * d20) * invDenom;
	
	// Intersection with triangle's plane but outside triangle
	if (a < -0.000f || b < -0.000f || a + b > 1) {
		return infinity;
	}

	return t;
}


//return the color of your pixel.
Vec3Df performRayTracing(const Vec3Df & origin, const Vec3Df & dest)
{
	int index = -1;
	double infinity = std::numeric_limits<double>::infinity();
	double nearest = infinity;

	for (int i = 0; i < MyMesh.triangles.size(); i++) {
		Triangle &triangle = MyMesh.triangles[i];
		auto dir = dest - origin;
		dir.normalize();
		double distance = intersection(origin, dir, triangle);
		if (distance < nearest) {
			nearest = distance;
			index = i;
		}
	}

	if (index != -1) {
		int materialIndex = MyMesh.triangleMaterials[index];
		Material &material = MyMesh.materials[materialIndex];
		return material.Kd();
	}

	return Vec3Df();
}


void yourDebugDraw()
{
	//draw open gl debug stuff
	//this function is called every frame

	//let's draw the mesh
	MyMesh.draw();
	
	//let's draw the lights in the scene as points
	glPushAttrib(GL_ALL_ATTRIB_BITS); //store all GL attributes
	glDisable(GL_LIGHTING);
	glColor3f(1,1,1);
	glPointSize(10);
	glBegin(GL_POINTS);
	for (int i=0;i<MyLightPositions.size();++i)
		glVertex3fv(MyLightPositions[i].pointer());
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
	glVertex3fv(MyLightPositions[0].pointer());
	glEnd();
	glPopAttrib();
	
	//draw whatever else you want...
	////glutSolidSphere(1,10,10);
	////allows you to draw a sphere at the origin.
	////using a glTranslate, it can be shifted to whereever you want
	////if you produce a sphere renderer, this 
	////triangulated sphere is nice for the preview
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
void yourKeyboardFunc(char t, int x, int y, const Vec3Df & rayOrigin, const Vec3Df & rayDestination)
{

	//here, as an example, I use the ray to fill in the values for my upper global ray variable
	//I use these variables in the debugDraw function to draw the corresponding ray.
	//try it: Press a key, move the camera, see the ray that was launched as a line.
	testRayOrigin=rayOrigin;	
	testRayDestination=rayDestination;
	
	// do here, whatever you want with the keyboard input t.
	
	//...
	
	
	std::cout<<t<<" pressed! The mouse was in location "<<x<<","<<y<<"!"<<std::endl;	
}
