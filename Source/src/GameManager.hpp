#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <list>
#include <conio.h>

#include <Component/SimpleScene.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include "GameEngine/GameObject.hpp"
#include "GameEngine/Camera.hpp"
#include "GameEngine/Lighting.hpp"
#include "GameEngine/Objects.hpp"

namespace Skyroads {
	namespace Constants {
		const std::vector<std::string> platformTypes{ "platform_red", "platform_green", "platform_yellow", "platform_orange", "platform_purple", "platform_blue", "platform_white" };
		const std::vector<std::string> shaderNames{ "Base", "UI", "ScreenShader", "Skybox", "Blur", "Spaceship", "EmmisiveTransparency", "Planet" };
		const std::vector<std::string> meshNames{ "box", "sphere"};
		const std::vector<std::string> textureNames{ "life", "skybox", "spaceship_window", "spaceship_exhaust", "icy", "jupiter", "mars", "neptune", "star_blue", "star_red", "uranus", "venus", "obstacle1", "obstacle2" };
		const std::vector<std::string> modelNames{ "platform", "spaceship" };

		const glm::vec3 lightPositionOffset = glm::vec3(0., 7.75f, 0.);
		const glm::vec3 playerStartingPosition = glm::vec3(0, 20.f, 35.f);

		const std::vector<float> lanesX{ -3.5f, 0.f, 3.5f };

		// Player constants
		const float maxSpeed = 0.125f;
		const float minSpeed = 0.0075f;
		const float speedStep = 0.001f;
		const float lateralSpeed = .75f;

		// Game Constants
		const float forcedSpeedTime = 5;		// In seconds
		const double powerAnimationTime = 2;	// In seconds
		const float maxLives = 3;
		const int maxPlatforms = 12;
		const int minPlatformGap = 5;
		const int maxPlatformGap = GameEngine::ObjectConstants::platformLength;
		const int simplePlatPercent = 60;
		const float noSpawnRange = 10.f;
		const float outOfBoundY = -3.5f;
		const int obstaclesPercent = 10;
		const int pointsPercent = 10;
		
		// Fuel constants
		const float maxFuel = 100.f;
		const float fuelGain = 0.33f * maxFuel;
		const float fuelLoss = 0.10f * maxFuel;
		const float fuelFlow = 2.5f;									// The "fuelFlow" factor
		const glm::vec3 fuelbarScale = glm::vec3(0.07, 1.9f, 1);		// The maximum scale/size of the fuelbar
		const float fuelbarsDiff = 0.01;

		// Camera constants
		const float minFov = 60.f;
		const float maxFov = 90.f;

		// Decoration constants
		const int maxDecorations = 16;
		const int maxStars = 2;
		const float starPercent = 5.f;
		const float minZOffset = 5.f;
		const float maxZOffset = 30.f;
		const float despawnDistance = 10.f;
		const float maxDecY = 15.f;
		const float minDecXOff = 6.5f;
		const float maxDecXOff = 35.f;

		// Rendering constants
		const float gamma = 1.2f;
		const float exposure = 0.5f;
		const unsigned int multisamples = 16;
		const unsigned int blur_amount = 10;	// Blur iterations
	};

	// Defines variables used in the game logic
	struct GameState {
		struct CameraSettings {
			float cameraFOV = 75.f;
			bool cameraMode = true;
			glm::vec2 cameraRotation = glm::vec2(0);
			float distanceToTarget = 2.25f;
		};
		CameraSettings cameraSettings;

		struct PlayerState {
			float fuel = Constants::maxFuel;
			bool isFullSpeed = false;
			double forcedSpeedStart = 0;	// The start time of the forced speed effect
			float lives = 1;
			float playerSpeed = 0.05f;
			float oldPlayerSpeed = 0.05f;   // The speed of the player before the forced speed effect
		};
		PlayerState playerState;

		float points = 0.f;
		float collected = 0;	// How many good objects he has collected

		// Data related to the platforms and decorations
		std::vector<float> nextPlatformSpawn = {Constants::playerStartingPosition.z, Constants::playerStartingPosition.z + 1, Constants::playerStartingPosition.z };
		int platformCount = 0;
		int decorationCount = 0;
		int starsCount = 0;
		bool gameStarted;
		int decorationLeftZ = 0;
		int decorationRightZ = 0;
	};

	class GameManager : public SimpleScene
	{
	public:
		GameManager();
		~GameManager();
		void Init() override;
		
		void addGameObject(GameEngine::GameObject object);

		/// <summary>
		/// Get the game object with a specific id
		/// </summary>
		/// <param name="id">The id of the game object</param>
		/// <returns>The game object</returns>
		GameEngine::GameObject* getGameObject(const long int id);

	private:
		/// <summary>
		/// A map of all the gameobjects, using the object id as a key
		/// </summary>
		std::unordered_map<long int, GameEngine::GameObject> gameObjects;
		std::unordered_map<std::string, Texture2D*> textures;
		GameEngine::GameObject skybox;

		GameEngine::Camera* camera;
		GameState gameState;

		std::vector<GameEngine::Light> permanentLights;		// Player + Ambient lights

		// Different buffers used for rendering
		unsigned int msaa_framebuffer, fx_framebuffer, pp_framebuffers[2];
		unsigned int msaa_colorbuffers[2], fx_colorbuffers[2], pp_colorbuffers[2];
		unsigned int msaa_renderbuffer;

		void LoadShader(std::string name, std::string shadersPath);
		void LoadMesh(std::string name, std::string meshesPath);
		void LoadTexture(std::string name, std::string extension, std::string texturesPath);

		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		/// <summary>
		/// Update the camera data
		/// </summary>
		void UpdateCamera();

		/// <summary>
		/// Update the player data
		/// </summary>
		void UpdatePlayer();

		/// <summary>
		/// Update all the data related to the game logic
		/// </summary>
		void UpdateGameState(const float deltaTime);

		/// <summary>
		/// Render the UI
		/// </summary>
		void RenderUI();

		/// <summary>
		/// Check collisions and update the game state
		/// </summary>
		/// <param name="collided">A vector with the id's of the collided objects</param>
		void CheckCollisions(std::vector<int> collided);

		/// <summary>
		/// Compute the score
		/// </summary>
		void ComputeScore();

		/// <summary>
		/// Function that handles the game end
		/// </summary>
		void GameOver();

		/// <summary>
		/// Spawn/Remove platforms/obstacles from the game
		/// </summary>
		void PlatformManagement();

		/// <summary>
		/// Spawn/Remove decorative elements from the game
		/// </summary>
		void DecorationManagement();

		/// <summary>
		/// Initialise the framebuffers
		/// </summary>
		void InitFramebuffers();

		/// <summary>
		/// Apply postprocessing
		/// </summary>
		void PostProcessing();

		/// <summary>
		/// Render the skybox
		/// </summary>
		void RenderSkybox();

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	};
}
