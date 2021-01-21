#include "Transform.hpp"

glm::mat4 GameEngine::Translate(glm::mat4 mat, glm::vec3 pos)
{
	glm::mat4 tranMatrix(
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		pos.x, pos.y, pos.z, 1.f
	);

	return mat * tranMatrix;
}

glm::mat4 GameEngine::Scale(glm::mat4 mat, glm::vec3 scale)
{
	glm::mat4 scaleMatrix(
		scale.x, 0.f, 0.f, 0.f,
		0.f, scale.y, 0.f, 0.f,
		0.f, 0.f, scale.z, 0.f,
		0.f, 0.f, 0.f, 1.f
	);

	return mat * scaleMatrix;
}
