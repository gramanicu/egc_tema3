#include "Camera.hpp"

GameEngine::Camera::Camera()
{
	position = glm::vec3(0, 5.f, 30.f);
	forward = glm::vec3(0, 0, -1);
	up = glm::vec3(0, 1, 0);
	right = glm::vec3(1, 0, 0);
	distanceToTarget = 2;
}

GameEngine::Camera::Camera(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up)
{
	Set(position, center, up);
}

void GameEngine::Camera::Set(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up)
{
	this->position = position;
	forward = glm::normalize(center - position);
	right = glm::cross(forward, up);
	this->up = glm::cross(right, forward);
}

void GameEngine::Camera::MoveForward(float distance)
{
	glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
	position += dir * distance;
}

void GameEngine::Camera::TranslateForward(float distance)
{
	position += glm::normalize(forward) * distance;
}

void GameEngine::Camera::TranslateUpward(float distance)
{
	position += glm::normalize(up) * distance;
}

void GameEngine::Camera::TranslateRight(float distance)
{
	glm::vec3 dir = glm::normalize(right);
	position += dir * distance;
}

void GameEngine::Camera::RotateFirstPerson_OX(float angle)
{
	forward = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, right) * glm::vec4(forward, 1)));
	up = glm::normalize(glm::cross(right, forward));
}

void GameEngine::Camera::RotateFirstPerson_OY(float angle)
{
	forward = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0)) * glm::vec4(forward, 1)));
	right = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 1)));
	up = glm::normalize(glm::cross(right, forward));
}

void GameEngine::Camera::RotateFirstPerson_OZ(float angle)
{
	right = glm::normalize(glm::vec3(glm::rotate(glm::mat4(1), angle, forward) * glm::vec4(right, 1)));
	up = glm::normalize(glm::cross(right, forward));
}

void GameEngine::Camera::RotateThirdPerson_OX(float angle)
{
	TranslateForward(distanceToTarget);
	RotateFirstPerson_OX(angle);
	TranslateForward(-distanceToTarget);
}

void GameEngine::Camera::RotateThirdPerson_OY(float angle)
{
	TranslateForward(distanceToTarget);
	RotateFirstPerson_OY(angle);
	TranslateForward(-distanceToTarget);
}

void GameEngine::Camera::RotateThirdPerson_OZ(float angle)
{
	TranslateForward(distanceToTarget);
	RotateFirstPerson_OZ(angle);
	TranslateForward(-distanceToTarget);
}

glm::mat4 GameEngine::Camera::GetViewMatrix()
{
	// Returns the View Matrix
	return glm::lookAt(position, position + forward, up);
}

glm::vec3 GameEngine::Camera::GetTargetPosition()
{
	return position + forward * distanceToTarget;
}
