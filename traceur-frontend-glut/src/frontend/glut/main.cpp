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
#include <memory>
#include <tuple>
#include <thread>

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
#include <traceur/frontend/glut/observer.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "traqueboule.h"


// The rendering kernel to use
std::unique_ptr<traceur::MultithreadedKernel> kernel;
// The scene we want to render
std::unique_ptr<traceur::Scene> scene;
// The scene graph visitor to draw the scene.
std::unique_ptr<traceur::OpenGLSceneGraphVisitor> visitor;
// The exporter we use to export the result
std::unique_ptr<traceur::Exporter> exporter;

// Test rays
std::vector<std::tuple<glm::vec3, glm::vec3, const traceur::Primitive *, int>> rays;

// The default model to load
const std::string DEFAULT_MODEL_PATH = "assets/dodge.obj";

// Window settings
const unsigned int WindowSize_X = 800;  // resolution X
const unsigned int WindowSize_Y = 800;  // resolution Y
const float zNear = 0.01f;
const float zFar = 30.f;

/**
 * Initialises the front-end.
 *
 * @param[in] path The path to the model to load.
 */
void init(std::string &path)
{
	auto factory = traceur::make_factory<traceur::KDTreeSceneGraphBuilder>();
	auto loader = std::make_unique<traceur::ObjLoader>(std::move(factory));
	printf("[main] Loading model at path \"%s\"\n", path.c_str());
	scene = loader->load(path);
	scene->lights.push_back(getCameraPosition());
	printf("[main] Loaded scene with %zu nodes\n", scene->graph->size());

	int threads = std::thread::hardware_concurrency();
	int partitions = 64 * threads;

	kernel = std::make_unique<traceur::MultithreadedKernel>(
		std::make_shared<traceur::BasicKernel>(),
		threads,
		partitions
	);
	visitor = std::make_unique<traceur::OpenGLSceneGraphVisitor>(false);
	exporter = std::make_unique<traceur::PPMExporter>();
	kernel->add_observer(std::make_shared<traceur::ProgressObserver>());
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
			.perspective(glm::radians(50.f), viewport.z / viewport.w, zNear, zFar);

	// Render the scene and capture the result
	auto result = kernel->render(*scene, camera);

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
		for (auto &light : scene->lights) {
			glVertex3fv(glm::value_ptr(light));
		}
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
		for (auto &ray : rays) {
			auto primitive = std::get<2>(ray);
			if (primitive) {
				glColor3fv(glm::value_ptr(primitive->material->diffuse));
			} else {
				glColor3f(0, 1, std::get<3>(ray) / 25);
			}
			glVertex3fv(glm::value_ptr(std::get<0>(ray)));
			glVertex3fv(glm::value_ptr(std::get<1>(ray)));
		}
	glEnd();
	glPointSize(10);
	glBegin(GL_POINTS);
		for (auto &light : scene->lights) {
			glVertex3fv(glm::value_ptr(light));
		}
	glEnd();
	glPopAttrib();
}

/**
 * This method is invoked for every image of the screen once.
 */
void animate()
{
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
	gluPerspective (50, (float)w/h, 0.01, 10);
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
	destination = glm::unProject(glm::vec3(x_I, y_new, 1), model, projection, viewport);
}

/* Forward Declaration */
bool computeTestRays(const traceur::Ray &, int);

/**
 * Computes the reflection ray given the hit context.
 *
 * @param[in] ray The ray to use.
 * @param[in] hit The hit structure to use.
 * @param[in] depth The depth of the recursion.
 */
void computeReflection(const traceur::Ray &ray, const traceur::Hit &hit, int depth)
{
	auto reflect = glm::reflect(ray.direction, hit.normal);
	bool intersects = computeTestRays(traceur::Ray(
		hit.position + reflect * 0.000001f,
		reflect
	), depth + 1);

	// Also show reflection rays that do not intersect a primitive
	if (!intersects) {
		rays.emplace_back(hit.position, hit.position + reflect, hit.primitive, depth + 1);
	}
}

/**
 * Computes the refraction ray given the hit context.
 *
 * @param[in] ray The ray to use.
 * @param[in] hit The hit structure to use.
 * @param[in] depth The depth of the recursion.
 */
void computeRefraction(const traceur::Ray &ray, const traceur::Hit &hit, int depth)
{
	float sourceDestRefraction;
	glm::vec3 refractionNormal;

	if(glm::dot(hit.normal, ray.direction) < 0.f) {
		// enter material
		sourceDestRefraction = 1.f / hit.primitive->material->opticalDensity;
		refractionNormal = hit.normal;
	} else {
		// exit material
		sourceDestRefraction = hit.primitive->material->opticalDensity / 1.f;
		refractionNormal = - hit.normal;
	}

	glm::vec3 refract = glm::refract(ray.direction, refractionNormal,
									 sourceDestRefraction);

	if (!glm::isnan(refract).length()) {
		traceur::Hit next(hit);
		// Full internal ray reflection
		next.normal = refractionNormal;
		return computeReflection(ray, next, depth + 1);
	}
	rays.emplace_back(hit.position, hit.position +  0.000001f * refract, hit.primitive, depth);
}

/**
 * Computes the test rays to draw given a ray.
 *
 * @param[in] ray The ray to use.
 * @param[in] depth The depth of the recursion.
 * @return <code>true</code> if there was an intersection, <code>false</code>
 * otherwise.
 */
bool computeTestRays(const traceur::Ray &ray, int depth)
{
	/* Allow max recursion depth of ten */
	if (depth > 10) {
		return false;
	}

	traceur::Hit hit;
	if (scene->graph->intersect(ray, hit)) {
		rays.emplace_back(ray.origin, hit.position, hit.primitive, depth);
		computeReflection(ray, hit, depth);
		computeRefraction(ray, hit, depth);
		return true;
	}

	return false;
}

/**
 * Computes the test rays for the given position of the screen.
 *
 * @param[in] x The x-coordinate on the screen.
 * @param[in] y The y-coordinate on the screen.
 */
void computeTestRays(int x, int y)
{
	glm::vec3 origin, destination;
	// Produce the ray for the current mouse position
	produceRay(x, y, origin, destination);
	rays.clear();
	computeTestRays(traceur::Ray(origin, destination - origin), 0);
}

/**
 * This method is invoked on keyboard input.
 */
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
	case 'L':
		// Add/update a light based on the camera position.
		scene->lights.push_back(getCameraPosition());
		break;
	case 'l':
		scene->lights[scene->lights.size() - 1] = getCameraPosition();
		break;
	case 'r':
		// Render the current scene
		render();
		break;
	case 'b':
		// Enable/disable bounding boxes visuals
		visitor->draw_bounding_box = !visitor->draw_bounding_box;
		break;
	case 't':
		// Draw test ray
		computeTestRays(x, y);
		break;
	case 27:
		exit(0);
	}
}

