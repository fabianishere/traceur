#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <ctime>

#include <glm/glm.hpp>
#include <traceur/core/kernel/ray.hpp>

#include "raytracing.h"
#include "traqueboule.h"
#include "imageWriter.h"


// This is the main application
// Most of the code in here, does not need to be modified.
// It is enough to take a look at the function "drawFrame",
// in case you want to provide your own different drawing functions
glm::vec3 MyCameraPosition;

// MyLightPositions stores all the light positions to use
// for the ray tracing. Please notice, the light that is 
// used for the real-time rendering is NOT one of these, 
// but following the camera instead.
std::vector<glm::vec3> MyLightPositions;

const unsigned int WindowSize_X = 800;  // resolution X
const unsigned int WindowSize_Y = 800;  // resolution Y

/**
 * Main function, which is drawing an image (frame) on the screen
*/
void drawFrame()
{
	yourDebugDraw();
}

//animation is called for every image on the screen once
void animate()
{
	MyCameraPosition = getCameraPosition();
	glutPostRedisplay();
}

/* Forward declarations */
void display(void);
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);

/**
 * The main entry point of the program.
 *
 * @param[in] argc The number of command line arguments passed to this program.
 * @param[in] argv The command line arguments passed to this program.
 */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	// Framebuffer setup
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	// Positioning and size of window
	glutInitWindowPosition(200, 100);
	glutInitWindowSize(WindowSize_X,WindowSize_Y);
	glutCreateWindow(argv[0]);

	// Initialize viewpoint
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -4);
	tbInitTransform();     // This is for the trackball, please ignore
	tbHelp();             // idem
	MyCameraPosition = getCameraPosition();

	// Activate the light following the camera
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	int LightPos[4] = {0, 0, 2, 0};
	int MatSpec[4] = {1, 1, 1, 1};
	glLightiv(GL_LIGHT0, GL_POSITION, LightPos);

	// Normals will be normalized in the graphics pipeline
	glEnable(GL_NORMALIZE);
	// Clear color of the background is black.
	glClearColor(0.0, 0.0, 0.0, 0.0);

	
	// Activate rendering modes
	//activate depth test
	glEnable(GL_DEPTH_TEST); 
	//draw front-facing triangles filled
	//and back-facing triangles as wires
	glPolygonMode(GL_FRONT,GL_FILL);
	glPolygonMode(GL_BACK,GL_LINE);
	//interpolate vertex colors over the triangles
	glShadeModel(GL_SMOOTH);


	// glut setup... to ignore
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutMouseFunc(tbMouseFunc);    // trackball
	glutMotionFunc(tbMotionFunc);  // uses mouse
	glutIdleFunc(animate);


	init();

	//main loop for glut... this just runs your application
	glutMainLoop();
	return 0;
}

/**
 * OpenGL setup - functions do not need to be changed! 
 * you can SKIP AHEAD TO THE KEYBOARD FUNCTION
 */
void display(void)
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	tbVisuTransform();
	drawFrame();
	glutSwapBuffers();
	glPopAttrib();
}

/**
 * Handle changes in window size.
 */
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho (-1.1, 1.1, -1.1,1.1, -1000.0, 1000.0);
	gluPerspective (50, (float)w/h, 0.01, 10);
	glMatrixMode(GL_MODELVIEW);
}

/**
 * Transform the x and y position on the screen into the corresponding 3D world 
 * position.
 */
void produceRay(int x_I, int y_I, glm::vec3 &origin, glm::vec3 &destination)
{
		int viewport[4];
		double modelview[16];
		double projection[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview); //recuperer matrices
		glGetDoublev(GL_PROJECTION_MATRIX, projection); //recuperer matrices
		glGetIntegerv(GL_VIEWPORT, viewport);//viewport
		int y_new = viewport[3] - y_I;

		double x, y, z;
		gluUnProject(x_I, y_new, 0, modelview, projection, viewport, &x, &y, &z);
		origin = glm::vec3(x, y, z);
		
		gluUnProject(x_I, y_new, 1, modelview, projection, viewport, &x, &y, &z);
		destination = glm::vec3(x, y, z);
}

/**
 * This method is invoked on keyboard input.
 */
void keyboard(unsigned char key, int x, int y)
{
    printf("[main.cpp] key %d pressed at %d,%d\n", key, x, y);
    fflush(stdout);
    switch (key) {
	// Add/update a light based on the camera position.
	case 'L':
		MyLightPositions.push_back(getCameraPosition());
		break;
	case 'l':
		MyLightPositions[MyLightPositions.size() - 1] = getCameraPosition();
		break;
	case 'r':
	{
		// Pressing r will launch the raytracing.
		printf("[main.cpp] tracing scene\n");

		//Setup an image with the size of the current image.
		Image result(WindowSize_X, WindowSize_Y);
		
		//produce the rays for each pixel, by first computing
		//the rays for the corners of the frustum.
		glm::vec3 origin00, dest00;
		glm::vec3 origin01, dest01;
		glm::vec3 origin10, dest10;
		glm::vec3 origin11, dest11;
		glm::vec3 origin, dest;


		produceRay(0,0, origin00, dest00);
		produceRay(0,WindowSize_Y-1, origin01, dest01);
		produceRay(WindowSize_X-1,0, origin10, dest10);
		produceRay(WindowSize_X-1,WindowSize_Y-1, origin11, dest11);

		clock_t begin = std::clock();

		for (unsigned int y=0; y<WindowSize_Y;++y) {
			for (unsigned int x=0; x<WindowSize_X;++x) {
				//produce the rays for each pixel, by interpolating 
				//the four rays of the frustum corners.
				float xscale=1.0f-float(x)/(WindowSize_X-1);
				float yscale=1.0f-float(y)/(WindowSize_Y-1);

				origin=yscale*(xscale*origin00+(1-xscale)*origin10)+
					(1-yscale)*(xscale*origin01+(1-xscale)*origin11);
				dest=yscale*(xscale*dest00+(1-xscale)*dest10)+
					(1-yscale)*(xscale*dest01+(1-xscale)*dest11);

				//launch raytracing for the given ray.
				glm::vec3 rgb = performRayTracing(origin, dest);
				//store the result in an image 
				result.setPixel(x,y, RGBValue(rgb[0], rgb[1], rgb[2]));
			}
		}


		clock_t end = std::clock();
		double secs = double(end - begin) / CLOCKS_PER_SEC;
		printf("[main.cpp] traced scene in %f seconds\n", secs);
		
		result.writeImage("result.ppm");
		break;
	}
	case 27:
		exit(0);
	}

	// Produce the ray for the current mouse position
	glm::vec3 testRayOrigin, testRayDestination;
	produceRay(x, y, testRayOrigin, testRayDestination);

	yourKeyboardFunc(key,x,y, testRayOrigin, testRayDestination);
}

