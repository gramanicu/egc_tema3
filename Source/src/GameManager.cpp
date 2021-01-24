#include "GameManager.hpp"

#include <vector>
#include <queue>
#include <math.h>

/// <summary>
/// Map a value that is in a range to another range
/// </summary>
/// <param name="sourceNumber">The value</param>
/// <param name="fromA">Starting value of the first range</param>
/// <param name="fromB">End value of the first range</param>
/// <param name="toA">Starting value of the second range</param>
/// <param name="toB">End value of the second range</param>
/// <param name="decimalPrecision">The number of decimals to use</param>
/// <returns>The mapped value</returns>
double mapBetweenRanges(double sourceNumber, double fromA, double fromB, double toA, double toB, int decimalPrecision) {
	double deltaA = fromB - fromA;
	double deltaB = toB - toA;
	double scale = deltaB / deltaA;
	double negA = -1 * fromA;
	double offset = (negA * scale) + toA;
	double finalNumber = (sourceNumber * scale) + offset;
	int calcScale = (int)pow(10, decimalPrecision);
	return (double)round(finalNumber * calcScale) / calcScale;
}

using namespace Skyroads;

GameManager::GameManager()
{
	camera = new GameEngine::Camera();
	camera->Set(glm::vec3(0, 5.f, 30.f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
	camera->distanceToTarget = gameState.cameraSettings.distanceToTarget;
	camera->projectionMatrix = glm::perspective(RADIANS(gameState.cameraSettings.cameraFOV), window->props.aspectRatio, 0.01f, 200.f);
}

GameManager::~GameManager()
{
}

void GameManager::Init()
{
	// Load meshes
	for each (auto & name in Constants::meshNames) {
		LoadMesh(name, "Source/src/Meshes/");
	}

	// Create a quad for the framebuffers
	Mesh* mesh = GameEngine::CreateQuad();
	meshes[mesh->GetMeshID()] = mesh;

	// Load shaders
	for each (auto & name in Constants::shaderNames) {
		LoadShader(name, "Source/src/Shaders/");
	}

	// Load textures
	for each (auto & name in Constants::textureNames) {
		LoadTexture(name, ".png", "Source/src/Textures/");
	}

	// Load models
	for each (auto & name in Constants::modelNames) {
		LoadTexture(name, ".png", "Source/src/Models/");
		LoadMesh(name, "Source/src/Models/");
	}

	using namespace GameEngine;
	// Link the meshes, shaders & textures to the game objects
	GameObject::meshes = &meshes;
	GameObject::shaders = &shaders;
	GameObject::textures = &textures;

	// Initialize the player object
	{
		GameObject player("player", glm::vec3(Constants::playerStartingPosition));
		player.getRigidBody().state.drag_coef = 10.f;
		player.isInJump = true;
		addGameObject(player);
	}

	// Initialize the skybox
	{
		skybox = GameObject("skybox", glm::vec3(Constants::playerStartingPosition));
	}

	InitFramebuffers();
}

void GameManager::InitFramebuffers() {
	glEnable(GL_MULTISAMPLE);

	// -- MSAA framebuffer configuration --
	glGenFramebuffers(1, &msaa_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, msaa_framebuffer);

	// Create a color attachement texture
	glGenTextures(2, msaa_colorbuffers);
	for (uint i = 0; i < 2; ++i) {
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msaa_colorbuffers[i]);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Constants::multisamples, GL_RGBA16F, 2560, 1440, GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, msaa_colorbuffers[i], 0);
	}

	// Create a renderbuffer object for depth and stencil attachment
	glGenRenderbuffers(1, &msaa_renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, msaa_renderbuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, Constants::multisamples, GL_DEPTH24_STENCIL8, 2560, 1440);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msaa_renderbuffer);

	uint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	
	// Check if the framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer(MSAA) is not complete!\n";
		exit(-1);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// -- End MSAA framebuffer configuration --

	// -- Ping-Pong framebuffers configuration --
	glGenFramebuffers(2, pp_framebuffers);
	glGenTextures(2, pp_colorbuffers);

	for (uint i = 0; i < 2; ++i) {
		glBindFramebuffer(GL_FRAMEBUFFER, pp_framebuffers[i]);

		glBindTexture(GL_TEXTURE_2D, pp_colorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 2560, 1440, 0, GL_RGBA, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pp_colorbuffers[i], 0);
		// Check if the framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer(Ping-Pong) is not complete!\n";
			exit(-1);
		}
	}

	// -- End Ping-Pong framebuffers configuration

	// -- Post-Processing framebuffer configuration -- 
	glGenFramebuffers(1, &fx_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, fx_framebuffer);

	// Create a color attachement texture
	glGenTextures(2, fx_colorbuffers);
	for (uint i = 0; i < 2; ++i) {
		glBindTexture(GL_TEXTURE_2D, fx_colorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 2560, 1440, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, fx_colorbuffers[i], 0);
	}

	glDrawBuffers(2, attachments);

	// Check if the framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer(Post-Processing) is not complete!\n";
		exit(-1);
	}

	// -- End Post-Processing framebuffer configuration --
}

void GameManager::addGameObject(GameEngine::GameObject object)
{
	gameObjects[object.getID()] = object;
}

GameEngine::GameObject* GameManager::getGameObject(const long int id)
{
	return &(gameObjects[id]);
}

void GameManager::LoadShader(std::string name, std::string shadersPath)
{
	Shader* shader = new Shader(name.c_str());
	shader->AddShader(shadersPath + name + ".VS.glsl", GL_VERTEX_SHADER);
	shader->AddShader(shadersPath + name + ".FS.glsl", GL_FRAGMENT_SHADER);
	shader->CreateAndLink();
	shaders[shader->GetName()] = shader;
}

void GameManager::LoadTexture(std::string name, std::string extension, std::string texturesPath)
{
	std::string fullPath = texturesPath + name + extension;

	Texture2D* texture = new Texture2D();
	texture->Load2D(fullPath.c_str(), GL_REPEAT);
	textures[name] = texture;
}

void GameManager::LoadMesh(std::string name, std::string meshesPath)
{
	Mesh* mesh = new Mesh(name.c_str());
	mesh->LoadMesh(meshesPath, name + ".obj");
	meshes[mesh->GetMeshID()] = mesh;
}

void GameManager::FrameStart()
{
	// Bind to framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, msaa_framebuffer);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, 2560, 1440);
}

void GameManager::UpdateCamera() {
	camera->distanceToTarget = gameState.cameraSettings.distanceToTarget;

	// Update camera mode and position
	if (gameState.cameraSettings.cameraMode) {
		// 3rd Person
		camera->Set(gameObjects[0].getRigidBody().state.x + glm::vec3(0.f, .5f, camera->distanceToTarget), gameObjects[0].getRigidBody().state.x - glm::vec3(0, 1, 100), glm::vec3(0, 1, 0));
		gameObjects[0].isRendered(true);
		camera->RotateThirdPerson_OX(gameState.cameraSettings.cameraRotation.x);
		camera->RotateThirdPerson_OY(gameState.cameraSettings.cameraRotation.y);
	}
	else {
		// 1st Person
		camera->Set(gameObjects[0].getRigidBody().state.x, gameObjects[0].getRigidBody().state.x - glm::vec3(0, 1, 100), glm::vec3(0, 1, 0));
		gameObjects[0].isRendered(false);
		camera->RotateFirstPerson_OX(gameState.cameraSettings.cameraRotation.x);
		camera->RotateFirstPerson_OY(gameState.cameraSettings.cameraRotation.y);
	}

	// Map the camera FOV to the player speed
	gameState.cameraSettings.cameraFOV = (float)mapBetweenRanges(gameState.playerState.playerSpeed, Constants::minSpeed, Constants::maxSpeed, Constants::minFov, Constants::maxFov, 1);

	// Update the projection matrix
	camera->projectionMatrix = glm::perspective(RADIANS(gameState.cameraSettings.cameraFOV), window->props.aspectRatio, 0.01f, 200.f);
}

void GameManager::UpdatePlayer()
{
	float pSpeed = gameState.playerState.playerSpeed;

	// Move the player forward
	gameObjects[0].getRigidBody().state.x.z -= gameState.playerState.playerSpeed;

	if (window->KeyHold(GLFW_KEY_A)) {
		// Move player left
		gameObjects[0].getRigidBody().addImpulse(-Constants::lateralSpeed, 0, 0);
	}
	else if (window->KeyHold(GLFW_KEY_D)) {
		// Move player right
		gameObjects[0].getRigidBody().addImpulse(Constants::lateralSpeed, 0, 0);
	}
	else if (window->KeyHold(GLFW_KEY_W)) {
		if (!gameState.playerState.isFullSpeed) {
			// Speed up
			pSpeed += Constants::speedStep;
			if (pSpeed > Constants::maxSpeed) {
				pSpeed = Constants::maxSpeed;
			}
		}
	}
	else if (window->KeyHold(GLFW_KEY_S)) {
		if (!gameState.playerState.isFullSpeed) {
			// Slow down
			pSpeed -= Constants::speedStep;
			if (pSpeed < Constants::minSpeed) {
				pSpeed = Constants::minSpeed;
			}
		}
	}

	// Check if the player has fallen
	if (gameObjects[0].getPosition().y < Constants::outOfBoundY) {
		GameOver();
	}

	gameState.playerState.playerSpeed = pSpeed;
}

void GameManager::UpdateGameState(const float deltaTime)
{
	// Update Player
	UpdatePlayer();

	// Update camera
	UpdateCamera();

	// Compute the current score
	ComputeScore();
	
	// Update the platforms
	PlatformManagement();

	// Check fuel state
	float speedFuelFactor = mapBetweenRanges(gameState.playerState.playerSpeed, Constants::minSpeed, Constants::maxSpeed, 0.5, 1.5, 1);
	gameState.playerState.fuel -= deltaTime * Constants::fuelFlow * speedFuelFactor;
	if (gameState.playerState.fuel <= 0) {
		// If all the fuel was used, a life is lost. If the game can go on
		// (at least 1 life remaining), reset the fuel to max

		gameState.playerState.lives--;
		if (gameState.playerState.lives > 0) {
			gameState.playerState.fuel = Constants::maxFuel;
		}
	}

	// Check lives
	if(gameState.playerState.lives <= 0) {
		// If all lives are lost, game over
		GameOver();
	}

	// Check if full speed should still be applied
	if (gameState.playerState.isFullSpeed && Engine::GetElapsedTime() - gameState.playerState.forcedSpeedStart >= Constants::forcedSpeedTime) {
		gameState.playerState.isFullSpeed = false;
		gameState.playerState.playerSpeed = gameState.playerState.oldPlayerSpeed;
	}
}

void Skyroads::GameManager::RenderUI()
{
	// Render the fuel bar
	GameEngine::GameObject fuelbar("fuelbar", glm::vec3(-0.9, 0, 0));
	GameEngine::GameObject ufuelbar("ufuelbar", glm::vec3(-0.9, 0, -1));

	ufuelbar.setScale(Constants::fuelbarScale + Constants::fuelbarsDiff);

	float percent = gameState.playerState.fuel / Constants::maxFuel;
	fuelbar.setScale(glm::vec3(Constants::fuelbarScale.x, Constants::fuelbarScale.y * percent, Constants::fuelbarScale.z));

	ufuelbar.Render2D();
	fuelbar.Render2D();

	// Render the number of lifes
	int lifesToRender = gameState.playerState.lives;
	glm::vec3 pos = glm::vec3(0.9, -0.9, 0);
	
	while (lifesToRender > 0) {
		GameEngine::GameObject life("life", pos);
		life.Render2D();

		lifesToRender--;
		pos.y += 0.15f;
	}
}

void GameManager::Update(float deltaTimeSeconds)
{
	// Render skybox
	RenderSkybox();

	UpdateGameState(deltaTimeSeconds);

	std::vector<GameEngine::GameObject*> gameObjectsVector;
	for (auto& object : gameObjects) {
		gameObjectsVector.push_back(&(object.second));
	}

	// Update Light
	glm::vec3 lightPosition = gameObjects[0].getRigidBody().state.x;
	
	// Back light
	std::vector<GameEngine::Light> lights;
	GameEngine::Light light = {
		GameEngine::LightType::Spot,
		lightPosition,
		glm::vec3(0, 0, 1),
		glm::vec3(0.25f),
		glm::vec3(1.f, 1.f, 5.f),
		glm::vec3(0.5f, 0.5f, 1.5f),
		1.0f, 0.09f, 0.032f,
		glm::cos(glm::radians(45.f)), glm::cos(glm::radians(90.f))
	};
	lights.push_back(light);

	// Front light
	light = {
		GameEngine::LightType::Spot,
		lightPosition,
		glm::vec3(0, 0, -1),
		glm::vec3(0.25f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		glm::vec3(1.5f, 1.5f, 1.f),
		1.0f, 0.014f, 0.0007f,
		glm::cos(glm::radians(25.f)), glm::cos(glm::radians(45.f))
	};
	lights.push_back(light);

	// Directional light
	light = {
		GameEngine::LightType::Directional,
		lightPosition,
		glm::vec3(0, -1, 0),
		glm::vec3(0.3f),
		glm::vec3(0.1f),
		glm::vec3(0.15f),
		1.0f, 1.f, 1.f,
		glm::cos(glm::radians(90.f)), glm::cos(glm::radians(90.f))
	};
	lights.push_back(light);
	
	// For every gameObject types (type.first = id, type.second = the object)
	for (auto& object : gameObjects) {
		// Update position
		object.second.UpdatePhysics(deltaTimeSeconds);

		// Check collisions
		CheckCollisions(object.second.ManageCollisions(gameObjectsVector, &gameObjects));

		// Render objects
		object.second.Render(camera, lights);
	};

	PostProcessing();	// Post-Processing is not applied to the UI or Skybox
	RenderUI();
}

void GameManager::RenderSkybox() {
	glm::vec3 lightPosition = gameObjects[0].getRigidBody().state.x + Constants::lightPositionOffset;
	std::vector<GameEngine::Light> lights;
	GameEngine::Light light = {
		GameEngine::LightType::Spot,
		lightPosition,
		glm::vec3(0, -1, 0),
		glm::vec3(0.25f),
		glm::vec3(0.8f, 1.f, 1.f),
		glm::vec3(0.8f, 1.f, 1.f),
		1.0f, 0.027f, 0.0028f,
		glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(25.f))
	};
	lights.push_back(light);

	skybox.setPosition(gameObjects[0].getRigidBody().state.x);
	skybox.Render(camera, lights);
}

void GameManager::PostProcessing() {
	// Store the current screen resolution
	glm::ivec2 resolution = window->GetResolution();
	
	// Copy data from the msaa framebuffer to the post-processing fx framebuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, msaa_framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fx_framebuffer);
	for (uint i = 0; i < 2; ++i) {
		glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
		glBlitFramebuffer(0, 0, 2560, 1440, 0, 0, 2560, 1440, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Copy data from the bright msaa color buffer to the pp buffers
	glBindFramebuffer(GL_READ_FRAMEBUFFER, msaa_framebuffer);
	for (uint i = 0; i < 2; ++i) {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pp_framebuffers[i]);
		glReadBuffer(GL_COLOR_ATTACHMENT1);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(0, 0, 2560, 1440, 0, 0, 2560, 1440, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
 
	// -- Apply the two-pass Gaussian Blur --
	glUseProgram(shaders["Blur"]->program);
	bool horizontal = true;
	glDisable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, 2560, 1440);
	for (uint i = 0; i < Constants::blur_amount; ++i) {
		// Activate the ping pong framebuffer
		// Each iteration, we will fill one of the "pp" framebuffers with the other's color
		glBindFramebuffer(GL_FRAMEBUFFER, pp_framebuffers[horizontal]);
		glUniform1f(glGetUniformLocation(shaders["Blur"]->program, "horizontal"), (GLint)horizontal);

		// Copy the color data to the ping-pong color buffer at the first iteration
		glBindTexture(GL_TEXTURE_2D, i == 0 ? pp_colorbuffers[horizontal] : pp_colorbuffers[!horizontal]);

		glBindVertexArray(meshes["quad"]->GetBuffers()->VAO);
		glDrawElements(meshes["quad"]->GetDrawMode(), static_cast<int>(meshes["quad"]->indices.size()), GL_UNSIGNED_SHORT, 0);

		// Switch framebuffers
		horizontal = !horizontal;
	}

	// -- Copy the "bloom" to the bright color buffer of the fx framebuffer
	glBindFramebuffer(GL_READ_FRAMEBUFFER, pp_framebuffers[!horizontal]);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fx_framebuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_COLOR_ATTACHMENT1);
	glBlitFramebuffer(0, 0, 2560, 1440, 0, 0, 2560, 1440, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// -- Blend the textures into the post-fx framebuffer and apply post-processing fx --
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glViewport(0, 0, resolution.x, resolution.y);

	glUseProgram(shaders["ScreenShader"]->program);
	glBindVertexArray(meshes["quad"]->GetBuffers()->VAO);

	// Load the two textures
	GLuint shader_program = shaders["ScreenShader"]->program;
	glUniform1i(glGetUniformLocation(shader_program, "screenTexture"), 0);
	glUniform1i(glGetUniformLocation(shader_program, "bloomTexture"), 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fx_colorbuffers[0]);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, fx_colorbuffers[1]);

	// Use the screen shader (Post-FX)
	glUniform1f(glGetUniformLocation(shader_program, "gamma"), (GLfloat)Constants::gamma);
	glUniform1f(glGetUniformLocation(shader_program, "exposure"), (GLfloat)Constants::exposure);
	glDrawElements(meshes["quad"]->GetDrawMode(), static_cast<int>(meshes["quad"]->indices.size()), GL_UNSIGNED_SHORT, 0);

	glEnable(GL_DEPTH_TEST);
}

void GameManager::FrameEnd()
{
}

void Skyroads::GameManager::CheckCollisions(std::vector<int> collided)
{
	if (collided.size() == 0) return;

	for (int id : collided) {
		std::string type = gameObjects[id].getType();

		// Make sure this is a platform
		if (type.rfind("platform_", 0) != 0) return;

		std::string color_string = type.substr(type.find("_") + 1);
		if (color_string == "red") {
			// Instant Loss
			GameOver();
		}
		else if (color_string == "yellow") {
			// Lose fuel
			gameState.playerState.fuel -= Constants::fuelLoss;
			gameObjects[0].setDistorted(Constants::powerAnimationTime);
		}
		else if (color_string == "orange") {
			// Speed up
			gameState.playerState.isFullSpeed = true;
			gameState.playerState.forcedSpeedStart = Engine::GetElapsedTime();
			gameState.playerState.oldPlayerSpeed = gameState.playerState.playerSpeed;
			gameState.playerState.playerSpeed = Constants::maxSpeed;
			gameObjects[0].setDistorted(Constants::forcedSpeedTime);
		}
		else if (color_string == "green") {
			// Gain fuel
			gameState.playerState.fuel += Constants::fuelGain;
			gameObjects[0].setDistorted(Constants::powerAnimationTime);
			if (gameState.playerState.fuel > Constants::maxFuel) {
				gameState.playerState.fuel = Constants::maxFuel;
			}
		}
		else if (color_string == "white") {
			if (gameState.playerState.lives < Constants::maxLives) {
				// Gain life
				gameState.playerState.lives += 1;
				gameObjects[0].setDistorted(Constants::powerAnimationTime);
			}
		}

		gameObjects[id].setType("platform_purple");
	}
}

void Skyroads::GameManager::ComputeScore()
{
	gameState.points = abs(gameObjects[0].getRigidBody().state.x.z - Constants::playerStartingPosition.z);
}

void Skyroads::GameManager::GameOver()
{
	std::cout << " --- Game Over --- " << "\n";
	std::cout << " Your score was : " << (int)gameState.points << "\n";
	std::cout << " Press any key to exit ...\n";
	int aux = _getch();
	exit(0);
}

void GameManager::PlatformManagement()
{
	// This function manages all the platforms, their spawning and removal
	// The platforms will be randomly spawned, many will be without effects.

	if (gameState.platformCount < Constants::maxPlatforms) {
		// Check the lane that hasn't spawn a platform in the longest time
		std::vector<float> nps = gameState.nextPlatformSpawn;
		int minLaneID = std::max_element(nps.begin(), nps.end()) - nps.begin(); // Max because the z is in descending order

		int platType = rand() % 100;

		if (gameState.platformCount < Constants::lanesX.size()) {
			platType = 0;	// First platforms should be simple
		}

		int platGap = rand() % (Constants::maxPlatformGap - Constants::minPlatformGap) + Constants::minPlatformGap;

		if (platType < Constants::simplePlatPercent) {
			// Simple platform
			GameEngine::GameObject platform("platform_blue", glm::vec3(Constants::lanesX[minLaneID], -0.125, nps[minLaneID]));
			addGameObject(platform);
		}
		else {
			// Effect platform
			platType = (int)mapBetweenRanges(platType, Constants::simplePlatPercent, 100, 0, 9, 1);

			if (platType < 1) {
				// Red platform - very few
				GameEngine::GameObject platform("platform_red", glm::vec3(Constants::lanesX[minLaneID], -0.125, nps[minLaneID]));
				addGameObject(platform);
			}
			else if (platType < 4) {
				// Yellow platform - some
				GameEngine::GameObject platform("platform_yellow", glm::vec3(Constants::lanesX[minLaneID], -0.125, nps[minLaneID]));
				addGameObject(platform);
			}
			else if (platType < 6) {
				// Green platform - few
				GameEngine::GameObject platform("platform_green", glm::vec3(Constants::lanesX[minLaneID], -0.125, nps[minLaneID]));
				addGameObject(platform);
			}
			else if (platType < 8) {
				// Orange platform - few
				GameEngine::GameObject platform("platform_orange", glm::vec3(Constants::lanesX[minLaneID], -0.125, nps[minLaneID]));
				addGameObject(platform);
			}
			else if (platType < 9) {
				// White platform - very few
				GameEngine::GameObject platform("platform_white", glm::vec3(Constants::lanesX[minLaneID], -0.125, nps[minLaneID]));
				addGameObject(platform);
			}
		}

		gameState.platformCount++;

		// Update the next platform spawn for that lane
		gameState.nextPlatformSpawn[minLaneID] -= GameEngine::ObjectConstants::platformLength + platGap;
	}

	// Check what platforms are out of sight (need to be removed)
	std::vector<int> toRemove;
	for (auto& object : gameObjects) {
		if (object.second.getType().rfind("platform_", 0) == 0) {
			if (object.second.getPosition().z > gameObjects[0].getPosition().z + GameEngine::ObjectConstants::platformLength / 2 + Constants::noSpawnRange) {
				toRemove.push_back(object.first);
			}
		}
	}
	
	// Remove the platforms
	for (auto& id : toRemove) {
		gameObjects.erase(id);
		gameState.platformCount--;
	}

	// Update the nextPlatformSpawn in case it got too low
	for (int i = 0; i < gameState.nextPlatformSpawn.size(); ++i) {
		// A next platform z is too low if the distance between it's center and the player's center (on the Z axis) is greater than the despawn range 
		if (gameState.nextPlatformSpawn[i] > gameObjects[0].getPosition().z - Constants::noSpawnRange) {
			gameState.nextPlatformSpawn[i] = gameObjects[0].getPosition().z - 2 * Constants::noSpawnRange;
		}
	}
}

void GameManager::OnInputUpdate(float deltaTime, int mods)
{
}

void GameManager::OnKeyPress(int key, int mods)
{

	switch (key) {
	case GLFW_KEY_C: {
		// Change camera modes
		gameState.cameraSettings.cameraMode = !gameState.cameraSettings.cameraMode;
		gameState.cameraSettings.cameraRotation = glm::vec2(0);
	} break;
	case GLFW_KEY_SPACE: {
		// Jump
		if (!gameObjects[0].isInJump) {
			gameObjects[0].isInJump = true;
			gameObjects[0].getRigidBody().state.v.y = 3.33f;
		}
	} break;
	/*case GLFW_KEY_KP_SUBTRACT: {
		gameState.cameraSettings.distanceToTarget += 0.25f;
		std::cout << "New zoom " << gameState.cameraSettings.distanceToTarget << "\n";
	} break;
	case GLFW_KEY_KP_ADD: {
		gameState.cameraSettings.distanceToTarget -= 0.25f;
		std::cout << "New zoom " << gameState.cameraSettings.distanceToTarget << "\n";
	} break;*/
	case GLFW_KEY_ESCAPE: {
		exit(-1);
	}
	}

}

void GameManager::OnKeyRelease(int key, int mods)
{
}

void GameManager::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float xLimit = 0.275;
		float yLimit = 0.5;

		float sensitivityOX = 0.001f;
		float sensitivityOY = 0.001f;
		glm::vec2 rotation = gameState.cameraSettings.cameraRotation;

		rotation += glm::vec2(-sensitivityOX * deltaY, -sensitivityOY * deltaX);

		if (rotation.x > xLimit) rotation.x = xLimit;
		if (rotation.x < -xLimit) rotation.x = -xLimit;
		if (rotation.y > yLimit) rotation.y = yLimit;
		if (rotation.y < -yLimit) rotation.y = -yLimit;

		gameState.cameraSettings.cameraRotation = rotation;
	}
}

void GameManager::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
		float distance = gameState.cameraSettings.distanceToTarget;
		float maxZoom = 2.5f;
		float minZoom = 5.0f;

		float sensitivity = 0.001f;

		distance += offsetX * sensitivity;

		if (distance > minZoom) distance = minZoom;
		if (distance < maxZoom) distance = maxZoom;

		gameState.cameraSettings.distanceToTarget = distance;

		std::cout << distance << "\n";
	}
}
