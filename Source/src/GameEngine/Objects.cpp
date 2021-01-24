#include "Objects.hpp"

Mesh* GameEngine::CreateQuad() {
	std::vector<glm::vec3> vertices
	{
		glm::vec3(-1.f,   1.f, 0.0f),
		glm::vec3(-1.f,  -1.f, 0.0f),
		glm::vec3(1.f, -1.f, 0.0f),
		glm::vec3(-1.f,  1.f, 0.0f),
		glm::vec3(1.f,  -1.f, 0.0f),
		glm::vec3(1.f,  1.f, 0.0f),
	};

	std::vector<glm::vec3> normals
	{
		glm::vec3(0, 1, 0),
		glm::vec3(0, 0, 1),
		glm::vec3(1, 0, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(1, 1, 0)
	};

	std::vector<glm::vec2> textureCoords
	{
		glm::vec2(0.f, 1.f),
		glm::vec2(0.f, 0.f),
		glm::vec2(1.f, 0.f),
		glm::vec2(0.f, 1.f),
		glm::vec2(1.f, 0.f),
		glm::vec2(1.f, 1.f)
	};

	std::vector<unsigned short> indices =
	{
		0, 1, 2,
		3, 4, 5
	};

	Mesh* mesh = new Mesh("quad");
	mesh->InitFromData(vertices, normals, textureCoords, indices);
	mesh->SetDrawMode(GL_TRIANGLES);
	return mesh;
}