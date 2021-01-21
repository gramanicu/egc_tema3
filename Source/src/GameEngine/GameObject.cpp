#include "GameObject.hpp"

#include <iostream>

long int GameEngine::GameObject::currentMaxID = 0;
std::unordered_map<std::string, Mesh*>* GameEngine::GameObject::meshes = nullptr;
std::unordered_map<std::string, Shader*>* GameEngine::GameObject::shaders = nullptr;
std::unordered_map<std::string, Texture2D*>* GameEngine::GameObject::textures = nullptr;

GameEngine::GameObject::GameObject() : id(-1), type(""), isInJump(false), distortedTime(0), _hasTexture(false) , _isRendered(true), position(glm::vec3(0)), mesh(nullptr), shader(nullptr), collider(nullptr), texture(nullptr) {};

GameEngine::GameObject::GameObject(const std::string& type, const glm::vec3& position) : type(type), position(position), distortedTime(0), mesh(nullptr), shader(nullptr), collider(nullptr), texture(nullptr) {
	id = currentMaxID++;
	_isRendered = true;
	_hasTexture = false;
	isInJump = false;

	if (type == "player") {
		scale = glm::vec3(ObjectConstants::playerHeight * 0.25f);
		scale *= glm::vec3(1, 1, -1);
		mesh = (*meshes)["spaceship"];
		/*
		scale = glm::vec3(ObjectConstants::playerHeight);
		mesh = (*meshes)["sphere"];*/
		shader = (*shaders)["Distorted"];
		lightingInfo = { 5.f, 0.5f, .25f };

		color = glm::vec3(1, 0, 0);
		collider = new Collider(id, position, ObjectConstants::playerHeight / 2);

		rigidbody.state.x = position;
		rigidbody.state.gravity_coef = .33f;
	}
	else if (type.rfind("platform_", 0) == 0) {
		scale = glm::vec3(1, 0.25f, ObjectConstants::platformLength);
		mesh = (*meshes)["platform"];
		shader = (*shaders)["Base"];
		texture = (*textures)["platform"];
		_hasTexture = true;
		lightingInfo = { 32.f, 0.99f, .3f };

		// Compute the Y component of the position
		this->position.y = ObjectConstants::platformTopHeight - scale.y / 2;

		collider = new Collider(id, this->position, scale);
		collider->affectsPhysics(true);

		rigidbody.state.x = this->position;
		rigidbody.physics_enabled = false;

		UpdatePlatformData();
	}
	else if (type == "sphere") {
		scale = glm::vec3(0.1);
		mesh = (*meshes)["sphere"];
		shader = (*shaders)["Base"];
		lightingInfo = { 5.f, 0.5f, .25f };

		color = glm::vec3(1, 0, 0);
		collider = new Collider(id, position, 0.1);

		rigidbody.state.x = this->position;
		rigidbody.physics_enabled = false;
	}
	else if (type == "fuelbar") {
		scale = glm::vec3(1, 1, 1);

		mesh = (*meshes)["box"];
		shader = (*shaders)["UI"];

		color = glm::vec3(0.9, 0.6, 0.2);
	}
	else if (type == "ufuelbar") {
		scale = glm::vec3(1, 1, 0.5);

		mesh = (*meshes)["box"];
		shader = (*shaders)["UI"];

		color = glm::vec3(0.5);
	}
	else if (type == "life") {
		scale = glm::vec3(0.125);

		mesh = (*meshes)["box"];
		shader = (*shaders)["UI"];

		color = glm::vec3(0.7, 0.1, 0.2);
		_hasTexture = true;
		texture = (*textures)["life"];
	}
}

void GameEngine::GameObject::UpdatePlatformData()
{
	// Make sure this is a platform
	if (type.rfind("platform_", 0) != 0) return;

	std::string color_string = type.substr(type.find("_") + 1);
	if (color_string == "red") {
		color = glm::vec3(1, 0, 0);
	}
	else if (color_string == "yellow") {
		color = glm::vec3(1, 1, 0);
	}
	else if (color_string == "orange") {
		color = glm::vec3(0.9, 0.6, 0.2);
	}
	else if (color_string == "green") {
		color = glm::vec3(0, 1, 0);
	}
	else if (color_string == "purple") {
		color = glm::vec3(0.5, 0.1, 0.4);
	}
	else if (color_string == "blue") {
		color = glm::vec3(0, 0, 1);
	}
	else if (color_string == "white") {
		color = glm::vec3(1);
	}
}

GameEngine::GameObject::GameObject(const GameObject& other)
{
	id = other.id;
	_isRendered = other._isRendered;
	_hasTexture = other._hasTexture;
	position = other.position;
	type = other.type;
	scale = other.scale;
	mesh = other.mesh;
	shader = other.shader;
	color = other.color;
	collider = other.collider;
	lightingInfo = other.lightingInfo;
	rigidbody = other.rigidbody;
	texture = other.texture;
}

void GameEngine::GameObject::Render(GameEngine::Camera *camera, const glm::vec3& lightLocation)
{
	glm::mat4 matrix = glm::mat4(1);
	matrix = Translate(matrix, position);
	matrix = Scale(matrix, scale);

	UpdatePlatformData();

	if (mesh == nullptr || shader == nullptr || !_isRendered) return;

	// Render the object
	glUseProgram(shader->program);

	// Bind MVP
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(camera->projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(matrix));
	
	// Bind Light-Data
	glm::vec3 cameraPos = camera->position;
	glUniform3f(shader->loc_eye_pos, cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform3f(glGetUniformLocation(shader->program, "light_position"), lightLocation.x, lightLocation.y, lightLocation.z);

	// Bind Material Data
	glUniform1f(glGetUniformLocation(shader->program, "material_shininess"), (GLfloat)lightingInfo.materialShine);
	glUniform1f(glGetUniformLocation(shader->program, "material_kd"), (GLfloat)lightingInfo.materialKd);
	glUniform1f(glGetUniformLocation(shader->program, "material_ks"), (GLfloat)lightingInfo.materialKs);
	glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(color));
	glUniform1i(glGetUniformLocation(shader->program, "is_distorted"), (distortedTime > 0));

	// Bind Texture Data
	if (_hasTexture) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->program, "texture1"), 0);
	}
	glUniform1i(glGetUniformLocation(shader->program, "has_texture"), _hasTexture);

	// Bind Other Data
	glUniform1f(glGetUniformLocation(shader->program, "time"), (GLfloat)Engine::GetElapsedTime());

	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

void GameEngine::GameObject::Render2D()
{
	glm::mat4 matrix = glm::mat4(1);
	matrix = glm::translate(matrix, position);
	matrix = glm::scale(matrix, scale);

	if (mesh == nullptr || shader == nullptr || !_isRendered) return;

	// Render the object
	glUseProgram(shader->program);

	// Bind MVP
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(matrix));

	glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(color));

	// Bind Texture Data
	if (_hasTexture) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
	}
	glUniform1i(glGetUniformLocation(shader->program, "has_texture"), _hasTexture);

	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

std::vector<int> GameEngine::GameObject::ManageCollisions(std::vector<GameObject*> collCheck, std::unordered_map<long int, GameEngine::GameObject>* allObjects) {
	// Only player collisions matter
	if (type != "player") return std::vector<int>(0);

	std::vector<GameObject*> platforms;
	for (auto& obj : collCheck) {
		if (obj->type.rfind("platform_", 0) == 0) {
			platforms.push_back(obj);
		}
	}

	// Get all the id's of the platforms this object collided with
	std::vector<int> collided = CollisionCheck(platforms);

	if (type == "player") {
		// Update the physics of the player if he collided with a platform
		// This will actually just mean that the player will "stick" to the platform
		if (collided.size() > 0) {
			rigidbody.state.v.y = 0;
			rigidbody.state.x.y = ObjectConstants::platformTopHeight + ObjectConstants::playerHeight / 2;
			isInJump = false;
		}
		return collided;
	}
	
	return std::vector<int>(0);
}

void GameEngine::GameObject::isRendered(const bool isRendered)
{
	_isRendered = isRendered;
}

glm::vec3 GameEngine::GameObject::getScale() const
{
	return scale;
}

void GameEngine::GameObject::setScale(const glm::vec3 newScale)
{
	scale = newScale;
}

glm::vec3 GameEngine::GameObject::getPosition() const
{
	return position;
}

void GameEngine::GameObject::setDistorted(const double time)
{
	distortedTime = time;
}

void GameEngine::GameObject::setPosition(const glm::vec3 newPosition)
{ 
	position = newPosition;
}

std::string GameEngine::GameObject::getType() const
{
	return type;
}

long int GameEngine::GameObject::getID() const
{
	return id;
}

void GameEngine::GameObject::setType(const std::string newType)
{
	type = newType;
}


std::vector<int> GameEngine::GameObject::CollisionCheck(std::vector<GameObject*> gameObjects)
{
	std::vector<Collider*> colArray;

	for (auto& obj : gameObjects) {
		// Do not include this object in the search
		if (obj->id != id) {
			colArray.push_back(obj->collider);
		}
	}

	return CollisionManager::getCollisions(*this->collider, colArray);
}

void GameEngine::GameObject::UpdatePhysics(const double deltaTime)
{
	if (distortedTime > 0) distortedTime -= deltaTime;

	PhysixEngine::UpdatePhysics(rigidbody, deltaTime);

	// Update the position from the physics engine
	position = rigidbody.state.x;
	collider->setPosition(position);
}

void GameEngine::GameObject::EnablePhysics()
{
	rigidbody.physics_enabled = true;
}

void GameEngine::GameObject::DisablePhysics()
{
	rigidbody.physics_enabled = false;
}

void GameEngine::GameObject::MovementType(GameEngine::PhysicsConstants::Motion_Type type)
{
	rigidbody.m_type = type;
}

void GameEngine::GameObject::MovementFunction(void(*f)(GameEngine::State& state, double time, double dt))
{
	rigidbody.m_func = f;
}

GameEngine::RigidBody& GameEngine::GameObject::getRigidBody()
{
	return rigidbody;
}

void GameEngine::GameObject::SetTexture(Texture2D& _texture) {
	texture = new Texture2D(_texture);
	_hasTexture = true;
}