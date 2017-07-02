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
#include <thread>
#include <map>
#include <future>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <traceur/core/kernel/basic.hpp>
#include <traceur/core/kernel/multithreaded.hpp>
#include <traceur/core/scene/graph/factory.hpp>
#include <traceur/core/scene/graph/vector.hpp>
#include <traceur/core/scene/graph/kdtree.hpp>
#include <traceur/loader/wavefront.hpp>
#include <traceur/exporter/ppm.hpp>

#include <traceur/frontend/glut/renderer.hpp>
#include <traceur/frontend/glut/progress.hpp>
#include <traceur/frontend/glut/preview.hpp>
#include <traceur/frontend/glut/debug.hpp>
#include <traceur/frontend/glut/trackball.hpp>

// The rendering kernel to use
std::unique_ptr<traceur::MultithreadedKernel> kernel;
// The scene we want to render
std::shared_ptr<traceur::Scene> scene;
// The scene graph visitor to draw the scene.
std::unique_ptr<traceur::GLUTSceneRenderer> renderer;
// The preview observer
std::shared_ptr<traceur::GLUTPreviewObserver> preview;
// The trackball to rotate around the scene
std::unique_ptr<traceur::GLUTTrackball> trackball;
// The debug tracer we use to preview test rays
std::unique_ptr<traceur::DebugTracer> debug;
// The exporter we use to export the result
std::unique_ptr<traceur::Exporter> exporter;
// The resulting film. This global variable prevents the film from going out
// of scope for the real-time preview render.
std::unique_ptr<traceur::Film> result;

// The default model to load
const std::string DEFAULT_MODEL_PATH = "assets/dodge.obj";

// Window settings
const unsigned int WindowSize_X = 800;  // resolution X
const unsigned int WindowSize_Y = 800;  // resolution Y
// Projection settings
const float zNear = 0.01f;
const float zFar = 30.f;

/**
 * Initialises the front-end.
 *
 * @param[in] viewport The viewport of the window.
 * @param[in] path The path to the model to load.
 */
void init(const glm::ivec4 &viewport, const std::string &path)
{
	auto factory = traceur::make_factory<traceur::KDTreeSceneGraphBuilder>();
	auto loader = std::make_unique<traceur::WavefrontLoader>(std::move(factory));
	printf("[main] Loading model at path \"%s\"\n", path.c_str());
	scene = loader->load(path);
	printf("[main] Loaded scene with %zu nodes\n", scene->graph->size());

	int threads = std::thread::hardware_concurrency();
	int partitions = 64 * threads;

	kernel = std::make_unique<traceur::MultithreadedKernel>(
		std::make_shared<traceur::BasicKernel>(),
		threads,
		partitions
	);

	// Set up the initial camera
	traceur::Camera camera = traceur::Camera(viewport)
		.perspective(glm::radians(50.f), viewport.z / viewport.w, zNear, zFar)
		.lookAt(glm::vec3(0, 0, 4), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));

	renderer = std::make_unique<traceur::GLUTSceneRenderer>(scene, false);
	preview = std::make_shared<traceur::GLUTPreviewObserver>();
	trackball = std::make_unique<traceur::GLUTTrackball>(camera, 0.2f);
	debug = std::make_unique<traceur::DebugTracer>(scene, 10);
	exporter = std::make_unique<traceur::PPMExporter>();

	kernel->add_observer(std::make_shared<traceur::ConsoleProgressObserver>(30));
	kernel->add_observer(preview);

	scene->lights.push_back(camera.position());
}

/**
 * Render the scene into a file.
 */
void render()
{
	// Render the scene and capture the result
	result = kernel->render(*scene, trackball->camera);

	// Export the result to a file
	exporter->write(*result, "result.ppm");
	printf("[main] Saved result to result.ppm\n");
}

/**
 * This function is invoked every frame to draw the image.
*/
void draw()
{
	// Render the scene graph
	renderer->render();
	// Draw test rays
	debug->render();
	// Draw real-time preview
	preview->render();

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
}

/**
 * This method is invoked for every image of the screen once.
 */
void animate()
{
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	trackball->mouse(button, state, x, y);
}

void motion(int x, int y)
{
	trackball->motion(x, y);
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
	glutMouseFunc(mouse);    // trackball
	glutMotionFunc(motion);  // uses mouse
	glutIdleFunc(animate);

	// Get the viewport of the window
	glm::ivec4 viewport;
	glGetIntegerv(GL_VIEWPORT, glm::value_ptr(viewport));

	std::string path = argc >= 2 ? argv[1] : DEFAULT_MODEL_PATH;
	init(viewport, path);

	// main loop for glut... this just runs your application
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
	trackball->transform();
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
	trackball->camera = trackball->camera.perspective(glm::radians(50.f), w / h, zNear, zFar);
	glLoadMatrixf(glm::value_ptr(trackball->camera.projection()));
	glMatrixMode(GL_MODELVIEW);
}

/**
 * This method is invoked on keyboard input.
 */
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
	case 'L':
		// Add/update a light based on the camera position.
		scene->lights.push_back(trackball->camera.position());
		break;
	case 'l':
		scene->lights[scene->lights.size() - 1] = trackball->camera.position();
		break;
	case 'r': {
		// Render the current scene on another thread, so that we do not freeze
		// the ui thread.
		std::thread([]() { render(); }).detach();
		break;
	}
	case 'b':
		// Enable/disable bounding boxes visuals
		renderer->draw_bounding_box = !renderer->draw_bounding_box;
		break;
	case 't':
		// Draw test ray
		debug->trace(x, y);
		break;
	case 'p':
		// Enable/disable real-time preview
		preview->enabled = !preview->enabled;
		break;
	case 27:
		exit(0);
	}
}
