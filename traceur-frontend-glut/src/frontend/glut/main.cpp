#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <ctime>
#include <chrono>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <traceur/core/kernel/basic.hpp>
#include <traceur/core/kernel/multithreaded.hpp>
#include <traceur/core/scene/graph/factory.hpp>
#include <traceur/core/scene/graph/vector.hpp>
#include <traceur/core/scene/graph/kdtree.hpp>
#include <traceur/loader/obj.hpp>
#include <traceur/exporter/ppm.hpp>

#include <traceur/frontend/glut/visitor.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "traqueboule.h"

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

//temporary variables
//these are only used to illustrate
//a simple debug drawing. A ray
glm::vec3 testRayOrigin;
glm::vec3 testRayDestination;

// The rendering kernel to use
std::unique_ptr<traceur::MultithreadedKernel> kernel;
// The scene we want to render
std::unique_ptr<traceur::Scene> scene;
// The scene graph visitor to draw the scene.
std::unique_ptr<traceur::OpenGLSceneGraphVisitor> visitor;
// The exporter we use to export the result
std::unique_ptr<traceur::Exporter> exporter;

// The default model to load
const std::string DEFAULT_MODEL_PATH = "assets/dodge.obj";

// Window settings
const unsigned int WindowSize_X = 800;  // resolution X
const unsigned int WindowSize_Y = 800;  // resolution Y

/**
 * Initialises the front-end.
 *
 * @param[in] path The path to the model to load.
 */
void init(std::string &path)
{
	//one first move: initialize the first light source
	//at least ONE light source has to be in the scene!!!
	//here, we set it to the current location of the camera
	MyLightPositions.push_back(MyCameraPosition);

	auto factory = traceur::make_factory<traceur::KDTreeSceneGraphBuilder>();
	auto loader = std::make_unique<traceur::ObjLoader>(std::move(factory));
	printf("[main] Loading model at path \"%s\"\n", path.c_str());
	scene = loader->load(path);

	kernel = std::make_unique<traceur::MultithreadedKernel>(std::make_shared<traceur::BasicKernel>(), 16);
	visitor = std::make_unique<traceur::OpenGLSceneGraphVisitor>(false);
	exporter = std::make_unique<traceur::PPMExporter>();
}

/**
 * Render the scene into a file.
 */
void render()
{
	// Get the viewport of the window
	glm::ivec4 viewport;
	glGetIntegerv(GL_VIEWPORT, glm::value_ptr(viewport));

	// Set up the camera
	traceur::Camera camera = traceur::Camera(viewport)
			.lookAt(getCameraPosition(), getCameraDirection(), getCameraUp())
			.perspective(glm::radians(50.f), 1, 0.01, 10);

	scene->camera = camera;
	scene->lights = MyLightPositions;

	printf("%zu\n", scene->lights.size());

	printf("[main] Rendering scene [%s]\n", kernel->name().c_str());

	// Time the ray tracing
	auto beginA = std::chrono::high_resolution_clock::now();
	auto beginB = std::clock();

	camera.viewport[2] = 1024;
	camera.viewport[3] = 1024;
	// Render the scene and capture the result
	auto result = kernel->render(*scene, camera);

	// Calculate the elapsed time
	auto endA = std::chrono::high_resolution_clock::now();
	auto endB = std::clock();
	double real = std::chrono::duration_cast<std::chrono::duration<double>>(endA - beginA).count();
	double cpu = double(endB - beginB) / CLOCKS_PER_SEC;
	printf("[main] Rendering done (cpu %.3fs, real %.3fs)\n", cpu, real);

	// Export the result to a file
	exporter->write(*result, "result.ppm");
	printf("[main] Saved result to result.ppm\n");
}


/**
 * This function is invoked every frame to draw the image.
*/
void draw()
{
	// Draw the loaded scene graph
	scene->graph->accept(*visitor);

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

/**
 * This method is invoked for every image of the screen once.
 */
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

	std::string path = argc >= 2 ? argv[1] : DEFAULT_MODEL_PATH;
	init(path);

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
	draw();
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
	gluPerspective (50, (float)w/h, 0.01, 100);
	glMatrixMode(GL_MODELVIEW);
}

/**
 * Transform the x and y position on the screen into the corresponding 3D world
 * position.
 */
void produceRay(int x_I, int y_I, glm::vec3 &origin, glm::vec3 &destination)
{
	glm::ivec4 viewport;
	glm::mat4x4 model;
	glm::mat4x4 projection;
	glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(model));
	glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(projection));
	glGetIntegerv(GL_VIEWPORT, glm::value_ptr(viewport));
	int y_new = viewport[3] - y_I;
	origin = glm::unProject(glm::vec3(x_I, y_new, 0), model, projection, viewport);
	destination = glm::unProject(glm::vec3(x_I, y_new, 0), model, projection,
						  viewport);
}

/**
 * This method is invoked on keyboard input.
 */
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
	case 'L':
		// Add/update a light based on the camera position.
		MyLightPositions.push_back(getCameraPosition());
		break;
	case 'l':
		MyLightPositions[MyLightPositions.size() - 1] = getCameraPosition();
		break;
	case 'r':
		// Render the current scene
		render();
		break;
	case 'b':
		// Enable/disable bounding boxes visuals
		visitor->draw_bounding_box = !visitor->draw_bounding_box;
		break;
	case 27:
		exit(0);
	}

	// Produce the ray for the current mouse position
	produceRay(x, y, testRayOrigin, testRayDestination);
}

