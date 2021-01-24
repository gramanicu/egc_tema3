#pragma once

#include <include/glm.h>
#include <Core/GPU/Mesh.h>
#include <Component/SimpleScene.h>

namespace GameEngine {
	namespace NormalVectors {
		const glm::vec3 up(0, 1, 0);
		const glm::vec3 down(0, -1, 0);
		const glm::vec3 forward(0, 0, 1);
		const glm::vec3 back(0, 0, -1);
		const glm::vec3 left(-1, 0, 0);
		const glm::vec3 right(1, 0, 0);
	}

	Mesh* CreateQuad();
	Mesh* CreateCube();
	Mesh* CreateSphere();
}