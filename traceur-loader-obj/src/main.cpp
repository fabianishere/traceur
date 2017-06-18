#include <traceur/core/scene/graph/vector.hpp>
#include <traceur/loader/obj.hpp>

#include <traceur/core/kernel/film.hpp>

#include <glm/ext.hpp>

#include <iostream>

int main()
{
	auto factory = traceur::make_factory<traceur::VectorSceneGraphBuilder>();
	auto loader = traceur::ObjLoader(std::move(factory));

	auto scene = loader.load("cube.obj");
	printf("info: successfully loaded scene \"cube.obj\"\n");

	auto film = new traceur::PartitionedFilm<traceur::DirectFilm>(16, 16, 2);


	return 0;
}
