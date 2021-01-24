#include "GameObject.hpp"

#include <iostream>

long int GameEngine::GameObject::currentMaxID = 0;
std::unordered_map<std::string, Mesh*>* GameEngine::GameObject::meshes = nullptr;
std::unordered_map<std::string, Shader*>* GameEngine::GameObject::shaders = nullptr;
std::unordered_map<std::string, Texture2D*>* GameEngine::GameObject::textures = nullptr;

GameEngine::GameObject::GameObject() : id(-1), type(""), isInJump(false), distortedTime(0), _hasTexture(false) , _isLight(false), _isRendered(true), position(glm::vec3(0)), mesh(nullptr), shader(nullptr), collider(nullptr), texture(nullptr) {};

GameEngine::GameObject::GameObject(const std::string& type, const glm::vec3& position) : type(type), position(position), distortedTime(0), mesh(nullptr), shader(nullptr), collider(nullptr), texture(nullptr) {
	id = currentMaxID++;
	_isRendered = true;
	_isLight = false;
	_hasTexture = false;
	isInJump = false;

	if (type == "player") {
		scale = glm::vec3(ObjectConstants::playerHeight * 0.25f);
		scale *= glm::vec3(1, 1, -1);
		mesh = (*meshes)["spaceship"];
		shader = (*shaders)["Spaceship"];
		texture = (*textures)["spaceship"];
		_hasTexture = true;
		material = {
			glm::vec3(0.f),
			glm::vec3(0.2f),
			glm::vec3(2.f),
			glm::vec3(0.35f),
			128.f
		};

		collider = new Collider(id, position, glm::vec3(ObjectConstants::playerHeight / 2));

		rigidbody.state.x = position;
		rigidbody.state.gravity_coef = .33f;
	}
	else if (type.rfind("platform_", 0) == 0) {
		scale = glm::vec3(1, 0.25f, ObjectConstants::platformLength);
		mesh = (*meshes)["cube"];
		shader = (*shaders)["EmmisiveTransparency"];
		texture = (*textures)["platform"];
		_hasTexture = true;
		material = {
			glm::vec3(1, 0, 0),
			glm::vec3(1, 0, 0),
			glm::vec3(32.f),
			glm::vec3(0.05f),
			16.f
		};

		// Compute the Y component of the position
		this->position.y = ObjectConstants::platformTopHeight - scale.y / 2;

		collider = new Collider(id, this->position, scale);
		collider->affectsPhysics(true);

		rigidbody.state.x = this->position;
		rigidbody.physics_enabled = false;

		UpdatePlatformData();
	}
	else if (type == "planet") {
		_hasTexture = true;
		_isLight = false;
		mesh = (*meshes)["c_sphere"];
		shader = (*shaders)["Planet"];
		material = {
			glm::vec3(1, 0, 0),
			glm::vec3(1, 0, 0),
			glm::vec3(32.f),
			glm::vec3(0.05f),
			16.f
		};

		collider = new Collider(id, this->position, 0.001f);
		collider->affectsPhysics(false);
		rigidbody.state.x = this->position;
		rigidbody.physics_enabled = true;
		rigidbody.state.drag_coef = 0.f;
		rigidbody.state.gravity_coef = 0.f;

		int planet = rand() % 6;
		switch (planet) {
		case 0: {
			scale = glm::vec3(0.5);
			texture = (*textures)["icy"];
			material.shininess = 2.5;
		} break;
		case 1: {
			scale = glm::vec3(0.5);
			texture = (*textures)["mars"];
			material.shininess = 1.5;
		} break;
		case 2: {
			scale = glm::vec3(1);
			texture = (*textures)["neptune"];
			material.shininess = 2.5;
		} break;
		case 3: {
			scale = glm::vec3(2);
			texture = (*textures)["jupiter"];
			material.shininess = 2.5;
		} break;
		case 4: {
			scale = glm::vec3(2);
			texture = (*textures)["uranus"];
			material.shininess = 1.5;
		} break;
		case 5: {
			scale = glm::vec3(1);
			texture = (*textures)["venus"];
			material.shininess = 2.5;
		} break;
		}
	}
	else if (type == "star") {
		scale = glm::vec3(4.);
		_hasTexture = true;
		_isLight = true;
		mesh = (*meshes)["c_sphere"];
		shader = (*shaders)["Planet"];
		material = {
			glm::vec3(1, 0, 0),
			glm::vec3(1, 0, 0),
			glm::vec3(32.f),
			glm::vec3(0.05f),
			96.f
		};

		collider = new Collider(id, this->position, 0.001f);
		collider->affectsPhysics(false);
		rigidbody.state.x = this->position;
		rigidbody.physics_enabled = true;
		rigidbody.state.drag_coef = 0.f;
		rigidbody.state.gravity_coef = 0.f;

		light = {
			GameEngine::LightType::Point,
			position,
			glm::vec3(0, -1, 0),
			glm::vec3(0.3f),
			glm::vec3(0.1f),
			glm::vec3(0.15f),
			1.0f, 0.014f, 0.0007f,
			glm::cos(glm::radians(90.f)), glm::cos(glm::radians(90.f))
		};

		int star = rand() % 2;
		switch (star) {
		case 0: {
			texture = (*textures)["star_blue"];
		} break;
		case 1: {
			texture = (*textures)["star_red"];
		} break;
		}
	}
	else if (type.rfind("obstacle_", 0) == 0) {
		scale = glm::vec3(1.);
		mesh = (*meshes)["cube"];
		shader = (*shaders)["EmmisiveTransparency"];
		_hasTexture = true;
		material = {
			glm::vec3(1, 0, 0),
			glm::vec3(1, 0, 0),
			glm::vec3(1.f),
			glm::vec3(0.30f),
			32.f
		};


		rigidbody.state.x = this->position;
		rigidbody.physics_enabled = false;

		std::string type_string = type.substr(type.find("_") + 1);
		if (type_string == "bad") {
			material.ambient = glm::vec3(1, 0, 0);
			material.emmisive = glm::vec3(122, 0, 0);
			texture = (*textures)["obstacle1"];
			scale = glm::vec3(10, 2, 1);
			collider = new Collider(id, position, scale);
			collider->affectsPhysics(true);
		}
		else if (type_string == "good") {
			material.ambient = glm::vec3(0.9, 0.6, 0.2);
			material.emmisive = glm::vec3(0, 122, 0);
			texture = (*textures)["obstacle2"];
			collider = new Collider(id, this->position, glm::vec3(1.2));
			collider->affectsPhysics(true);
		}
	}
	else if (type == "sphere") {
		scale = glm::vec3(0.1);
		mesh = (*meshes)["c_sphere"];
		shader = (*shaders)["Base"];
		material = {
			glm::vec3(0.f),
			glm::vec3(1, 0, 0),
			glm::vec3(5.f),
			glm::vec3(0.3f),
			.25f
		};
		collider = new Collider(id, position, 0.1);

		rigidbody.state.x = this->position;
		rigidbody.physics_enabled = false;
	}
	else if (type == "skybox") {
		scale = glm::vec3(200.f);
		mesh = (*meshes)["sphere"];
		shader = (*shaders)["Skybox"];
		texture = (*textures)["skybox"];
		_hasTexture = true;
		material = {
			glm::vec3(1.f),
			glm::vec3(1.f),
			glm::vec3(0.f),
			glm::vec3(0.f),
			.0f
		};

		rigidbody.state.x = this->position;
		rigidbody.physics_enabled = false;
	}
	else if (type == "fuelbar") {
		scale = glm::vec3(1, 1, 1);

		mesh = (*meshes)["box"];
		shader = (*shaders)["UI"];
		material = {
			glm::vec3(0.9, 0.6, 0.2),
			glm::vec3(0.9, 0.6, 0.2),
			glm::vec3(5.f),
			glm::vec3(0.3f),
			.25f
		};
	}
	else if (type == "ufuelbar") {
		scale = glm::vec3(1, 1, 0.5);

		mesh = (*meshes)["box"];
		shader = (*shaders)["UI"];

		material = {
			glm::vec3(0.5f),
			glm::vec3(0.5f),
			glm::vec3(5.f),
			glm::vec3(0.3f),
			.25f
		};
	}
	else if (type == "life") {
		scale = glm::vec3(0.125);

		mesh = (*meshes)["box"];
		shader = (*shaders)["UI"];

		material = {
			glm::vec3(0.7, 0.1, 0.2),
			glm::vec3(0.7, 0.1, 0.2),
			glm::vec3(5.f),
			glm::vec3(0.3f),
			.25f
		};
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
		material.ambient = glm::vec3(1, 0, 0);
		material.emmisive = glm::vec3(25.5, 0, 0);
	}
	else if (color_string == "yellow") {
		material.ambient = glm::vec3(1, 1, 0);
		material.emmisive = glm::vec3(25.5, 25.5, 0);
	}
	else if (color_string == "orange") {
		material.ambient = glm::vec3(0.9, 0.6, 0.2);
		material.emmisive = glm::vec3(25.9, 9.9, 7.1);
	}
	else if (color_string == "green") {
		material.ambient = glm::vec3(0.9, 0.6, 0.2);
		material.emmisive = glm::vec3(0, 25.5, 0);
	}
	else if (color_string == "purple") {
		material.ambient = glm::vec3(0.5, 0.1, 0.4);
		material.emmisive = glm::vec3(12.7, 2.5, 10.2);
	}
	else if (color_string == "blue") {
		material.ambient = glm::vec3(0, 0, 1);
		material.emmisive = glm::vec3(4.5, 5.5, 22.5);
	}
	else if (color_string == "white") {
		material.ambient = glm::vec3(1);
		material.emmisive = glm::vec3(25.5);
	}
}

GameEngine::GameObject::GameObject(const GameObject& other)
{
	id = other.id;
	_isLight = other._isLight;
	_isRendered = other._isRendered;
	_hasTexture = other._hasTexture;
	position = other.position;
	type = other.type;
	scale = other.scale;
	mesh = other.mesh;
	shader = other.shader;
	collider = other.collider;
	rigidbody = other.rigidbody;
	texture = other.texture;
	material = other.material;
	light = other.light;
}

void GameEngine::GameObject::Render(GameEngine::Camera* camera, const std::vector<Light>& lights)
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

	// Bind Lights Data
	glm::vec3 cameraPos = camera->position;
	glUniform3fv(shader->loc_eye_pos, 1, glm::value_ptr(cameraPos));
	glUniform1i(glGetUniformLocation(shader->program, "lights_count"), (GLint)lights.size());

	// Send the data for all the lights
	int id = 0;
	for (auto _light : lights) {
		std::string name = "lights[";
		glUniform1i(glGetUniformLocation(shader->program, (name + std::to_string(id) + "].type").c_str()), (GLint)_light.type);
		glUniform3fv(glGetUniformLocation(shader->program, (name + std::to_string(id) + "].position").c_str()), 1, glm::value_ptr(_light.position));
		glUniform3fv(glGetUniformLocation(shader->program, (name + std::to_string(id) + "].direction").c_str()), 1, glm::value_ptr(_light.direction));
		glUniform3fv(glGetUniformLocation(shader->program, (name + std::to_string(id) + "].ambient").c_str()), 1, glm::value_ptr(_light.ambient));
		glUniform3fv(glGetUniformLocation(shader->program, (name + std::to_string(id) + "].diffuse").c_str()), 1, glm::value_ptr(_light.diffuse));
		glUniform3fv(glGetUniformLocation(shader->program, (name + std::to_string(id) + "].specular").c_str()), 1, glm::value_ptr(_light.specular));
		glUniform1f(glGetUniformLocation(shader->program, (name + std::to_string(id) + "].constant").c_str()), (GLfloat)_light.constant);
		glUniform1f(glGetUniformLocation(shader->program, (name + std::to_string(id) + "].linear").c_str()), (GLfloat)_light.linear);
		glUniform1f(glGetUniformLocation(shader->program, (name + std::to_string(id) + "].quadratic").c_str()), (GLfloat)_light.quadratic);
		glUniform1f(glGetUniformLocation(shader->program, (name + std::to_string(id) + "].cutOff").c_str()), (GLfloat)_light.cutOff);
		glUniform1f(glGetUniformLocation(shader->program, (name + std::to_string(id) + "].outerCutOff").c_str()), (GLfloat)_light.outerCutOff);
		id++;
	}

	// Bind Material Data
	glUniform3fv(glGetUniformLocation(shader->program, "material.emmisive"), 1, glm::value_ptr(material.emmisive));
	glUniform3fv(glGetUniformLocation(shader->program, "material.ambient"), 1, glm::value_ptr(material.ambient));
	glUniform3fv(glGetUniformLocation(shader->program, "material.diffuse"), 1, glm::value_ptr(material.diffuse));
	glUniform3fv(glGetUniformLocation(shader->program, "material.specular"), 1, glm::value_ptr(material.specular));
	glUniform1f(glGetUniformLocation(shader->program, "material.shininess"), material.shininess);
	
	// Bind Texture Data
	if (_hasTexture) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->program, "texture1"), 0);
	}
	glUniform1i(glGetUniformLocation(shader->program, "has_texture"), _hasTexture);

	// Bind Other Data
	glUniform1f(glGetUniformLocation(shader->program, "time"), (GLfloat)Engine::GetElapsedTime());
	glUniform1i(glGetUniformLocation(shader->program, "is_distorted"), (distortedTime > 0));

	if (type == "player")
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, (*textures)["spaceship_window"]->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->program, "window_map"), 1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, (*textures)["spaceship_exhaust"]->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->program, "exhaust_map"), 2);

		// Spaceship shader data
		using namespace ObjectConstants;
		glUniform3fv(glGetUniformLocation(shader->program, "window_color_emm"), 1, glm::value_ptr(window_color_emm));
		glUniform3fv(glGetUniformLocation(shader->program, "exhaust_color_emm"), 1, glm::value_ptr(exhaust_color_emm));
	}

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

	glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(material.emmisive));

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
		using namespace ObjectConstants;
		if (collided.size() > 0 && rigidbody.state.x.y > -ObjectConstants::playerHeight / 4) {
			rigidbody.state.v.y = 0;
			rigidbody.state.x.y = ObjectConstants::platformTopHeight + ObjectConstants::playerHeight / 4;
			isInJump = false;
		}

		std::vector<GameObject*> obstacles;
		for (auto& obj : collCheck) {
			if (obj->type.rfind("obstacle_", 0) == 0) {
				obstacles.push_back(obj);
			}
		}

		std::vector<int> collided_obst = CollisionCheck(obstacles);

		for (auto& val : collided_obst) {
			collided.push_back(val);
		}

		return collided;		// Continue other collisions from here
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
	if (collider != nullptr) {
		collider->setDimensions(newScale);
	}
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

bool GameEngine::GameObject::getLight(Light* light_ptr) const
{
	if (_isLight) {
		*light_ptr = light;
		return true;
	}
	return false;
}

GameEngine::RigidBody& GameEngine::GameObject::getRigidBody()
{
	return rigidbody;
}

void GameEngine::GameObject::SetTexture(Texture2D& _texture) {
	texture = new Texture2D(_texture);
	_hasTexture = true;
}