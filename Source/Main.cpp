#include <ctime>
#include <iostream>

using namespace std;

#include <Core/Engine.h>
#include <src/GameManager.hpp>

int main(int argc, char **argv)
{
	srand((unsigned int)time(NULL));

	// Create a window property structure
	WindowProperties wp;
	wp.resolution = glm::ivec2(1280, 720);
	wp.fullScreen = false;
	wp.name = "Skyroads";
	wp.vSync = true;

	// Init the Engine and create a new window with the defined properties
	WindowObject* window = Engine::Init(wp);

	// Create a new 3D world and start running it
	World *world = new Skyroads::GameManager();
	world->Init();
	world->Run();

	// Signals to the Engine to release the OpenGL context
	Engine::Exit();

	return 0;
}