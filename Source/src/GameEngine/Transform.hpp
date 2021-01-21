#pragma once

#include <include/glm.h>

namespace GameEngine {
	glm::mat4 Translate(glm::mat4 mat, glm::vec3 pos);
	glm::mat4 Scale(glm::mat4 mat, glm::vec3 scale);

	// Rotation was not needed
}