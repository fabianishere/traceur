#ifndef RAYTRACING_Hjdslkjfadjfasljf
#define RAYTRACING_Hjdslkjfadjfasljf
#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <traceur/core/scene/scene.hpp>

//Welcome to your MAIN PROJECT...
//THIS IS THE MOST relevant code for you!
//this is an important file, raytracing.cpp is what you need to fill out
//In principle, you can do the entire project ONLY by working in these two files

extern std::vector<glm::vec3> MyLightPositions;
extern glm::vec3 MyCameraPosition; //currCamera
extern const unsigned int WindowSize_X;//window resolution width
extern const unsigned int WindowSize_Y;//window resolution height
extern unsigned int RayTracingResolutionX;  // largeur fenetre
extern unsigned int RayTracingResolutionY;  // largeur fenetre

//use this function for any preprocessing of the mesh.
void init();

//you can use this function to transform a click to an origin and destination
//the last two values will be changed. There is no need to define this function.
//it is defined elsewhere
void produceRay(int x_I, int y_I, glm::vec3 &origin, glm::vec3 &dest);


//your main function to rewrite
glm::vec3 performRayTracing(const traceur::Ray &);

//a function to debug --- you can draw in OpenGL here
void yourDebugDraw();

//want keyboard interaction? Here it is...
void yourKeyboardFunc(char t, int x, int y, const glm::vec3 &rayOrigin, const glm::vec3 &rayDestination);

#endif
