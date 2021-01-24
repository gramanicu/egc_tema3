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

Mesh* GameEngine::CreateCube() {
	// Credits: https://gist.github.com/prucha/866b9535d525adc984c4fe883e73a6c7

	// Mesh Constants
	std::vector<glm::vec3> corners
	{
		glm::vec3(-0.5f,   -0.5f, 0.5f),
		glm::vec3(0.5f,  -0.5f, 0.5f),
		glm::vec3(0.5f, -0.5f, -0.5f),
		glm::vec3(-0.5f,  -0.5f, -0.5f),
		glm::vec3(-0.5f,   0.5f, 0.5f),
		glm::vec3(0.5f,  0.5f, 0.5f),
		glm::vec3(0.5f, 0.5f, -0.5f),
		glm::vec3(-0.5f,  0.5f, -0.5f)
	};

	glm::vec2 uv00(0.f, 0.f);
	glm::vec2 uv10(1.f, 0.f);
	glm::vec2 uv01(0.f, 1.f);
	glm::vec2 uv11(1.f, 1.f);

	// Vertices
	std::vector<glm::vec3> vertices
	{
		corners[0], corners[1], corners[2], corners[3],
		corners[7], corners[4], corners[0], corners[3],
		corners[4], corners[5], corners[1], corners[0],
		corners[6], corners[7], corners[3], corners[2],
		corners[5], corners[6], corners[2], corners[1],
		corners[7], corners[6], corners[5], corners[4]
	};

	// Normals
	using namespace NormalVectors;
	std::vector<glm::vec3> normals
	{
		down, down, down, down,
		left, left, left, left,
		forward, forward, forward, forward,
		back, back, back, back,
		right, right, right, right,
		up, up, up, up
	};

	// Texture coordinates

	std::vector<glm::vec2> textureCoords
	{
		uv11, uv01, uv00, uv10,
		uv11, uv01, uv00, uv10,
		uv11, uv01, uv00, uv10,
		uv11, uv01, uv00, uv10,      
		uv11, uv01, uv00, uv10,
		uv11, uv01, uv00, uv10
	};

	// Faces
	std::vector<unsigned short> indices =
	{
		3, 1, 0,        3, 2, 1,
		7, 5, 4,        7, 6, 5,
		11, 9, 8,       11, 10, 9,
		15, 13, 12,     15, 14, 13,
		19, 17, 16,     19, 18, 17,
		23, 21, 20,     23, 22, 21
	};

	Mesh* mesh = new Mesh("cube");
	mesh->InitFromData(vertices, normals, textureCoords, indices);
	mesh->SetDrawMode(GL_TRIANGLES);
	return mesh;
}

Mesh* GameEngine::CreateSphere() {
	// Credits: http://www.songho.ca/opengl/gl_sphere.html
	// We suppose the radius = 1
	const uint stacks = 16;
	const uint sectors = 32;
	const double pi = 3.14159265358979323846264338327950288;

	float sectorStep = 2 * pi / sectors;
	float stackStep = pi / stacks;
	float sectorAngle, stackAngle;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned short> indices;

	for (uint i = 0; i <= stacks; ++i) {
		stackAngle = pi / 2 - (double)i * stackStep;
		float xy = cosf(stackAngle); // times radius (=1)
		float z = sinf(stackAngle); // same as abouve

		for (uint j = 0; j <= sectors; ++j) {
			sectorAngle = j * sectorStep;

			float x = xy * cosf(sectorAngle);
			float y = xy * sinf(sectorAngle);

			vertices.push_back(glm::vec3(x, y, z));
			normals.push_back(glm::vec3(x, y, z));
			uvs.push_back(glm::vec2((float)j / sectors, (float)i / stacks));
		}
	}
	// Compute indices
	int k1, k2;
	for (uint i = 0; i < stacks; ++i) {
		k1 = i * (sectors + 1);
		k2 = k1 + sectors + 1;
		for (uint j = 0; j < sectors; ++j, ++k1, ++k2) {
			if (i != 0) {
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}
			if (i != (stacks - 1)) {
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}
		}
	}

	Mesh* mesh = new Mesh("c_sphere");
	mesh->InitFromData(vertices, normals, uvs, indices);
	mesh->SetDrawMode(GL_TRIANGLES);
	return mesh;
}