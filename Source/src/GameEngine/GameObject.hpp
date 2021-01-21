#pragma once

#include <Core/Engine.h>
#include "Physics.hpp"
#include "CollisionManager.hpp"
#include "Camera.hpp"
#include "Transform.hpp"

namespace GameEngine {
	namespace Data {
		typedef struct _lightingData {
			float materialShine = 5.f;
			float materialKd = 0.5f;
			float materialKs = 0.5f;
		} lightingData;
	}

	namespace ObjectConstants {
		/// <summary>
		/// Where the top surface of the platform is placed
		/// </summary>
		const float platformTopHeight = 0.f;

		/// <summary>
		/// The height (scale) of the player (ball)
		/// </summary>
		const float playerHeight = 1.f;

		/// <summary>
		/// The Z size of the platform
		/// </summary>
		const float platformLength = 33.3f;
	}

	class GameObject
	{
	private:
		static long int currentMaxID;

		long int id;
		bool _isRendered;
		bool _hasTexture;
		std::string type;

		glm::vec3 position;
		glm::vec3 scale;

		Mesh *mesh;
		Shader *shader;
		RigidBody rigidbody;
		Collider *collider;
		Texture2D *texture;
		glm::vec3 color;
		Data::lightingData lightingInfo;

		/// <summary>
		/// How much the object must be in a distorted state (specifically, the player)
		/// </summary>
		double distortedTime;

		/// <summary>
		/// In case this object is a platform, it is possible that it's type will change (color). So, the color data and others things must be updated
		/// </summary>
		void UpdatePlatformData();

		/// <summary>
		/// Find all existing collisions between this object and the specified objects
		/// <param name="gameObjects">All the objects to be checked (this object is automatically ignored)</param>
		/// <returns>An array with the id's of all the objects this object is colliding with</returns>
		std::vector<int> CollisionCheck(std::vector<GameObject*> gameObjects);
	public:
		/// <summary>
		/// Variable used by the player game object
		/// </summary>
		bool isInJump;

		static std::unordered_map<std::string, Mesh*>* meshes;
		static std::unordered_map<std::string, Shader*>* shaders;
		static std::unordered_map<std::string, Texture2D*>* textures;

		/// <summary>
		/// A simple constructor
		/// </summary>
		GameObject();

		/// <summary>
		/// Constructor for a GameObject. In the case of platforms, the Y component of the position
		/// is ignored, and it will computed in such a way that the top of the platform is placed
		/// at "ObjectConstants::platformTopHeight"
		/// </summary>
		/// <param name="type">The type of the object</param>
		/// <param name="position">The position of the object</param>
		GameObject(const std::string& type, const glm::vec3& position);

		// Copy-Constructor
		GameObject(const GameObject& other);

		/// <summary>
		/// Renders the GameObject on the scene.
		/// </summary>
		/// <param name="camera">The camera used in the scene</param>
		/// <param name="lightLocation">The location of the light</param>
		void Render(GameEngine::Camera* camera, const glm::vec3& lightLocation);

		/// <summary>
		/// Renders the GameObject on the scene.
		/// </summary>
		void Render2D();

		/// <summary>
		/// Manage the collisions between this object and all the other game objects
		/// </summary>
		/// <param name="coll_check">All the objects to be checked (this object is automatically ignored)</param>
		/// <param name="allObjects">All the objects in the scene (pointer to the map)</param>
		/// <returns>A vector with the id's of all the objects this object is colliding with</returns>
		std::vector<int> ManageCollisions(std::vector<GameObject*> collCheck, std::unordered_map<long int, GameEngine::GameObject>* allObjects);

		/// <summary>
		/// Set if this object will be rendered
		/// </summary>
		/// <param name="isRendered"></param>
		void isRendered(const bool isRendered);

		/// <summary>
		/// Get the scale of the game object
		/// </summary>
		/// <returns>The scale</returns>
		glm::vec3 getScale() const;

		/// <summary>
		/// Set the scale of the game object
		/// </summary>
		/// <param name="newScale">The new scale</param>
		void setScale(const glm::vec3 newScale);

		/// <summary>
		/// Get the position of the game object
		/// </summary>
		/// <returns>The position</returns>
		glm::vec3 getPosition() const;

		/// <summary>
		/// Set the time this object must be in a distorted state (will use the distorted shader)
		/// </summary>
		/// <param name="time">The time</param>
		void setDistorted(const double time);

		/// <summary>
		/// Set the position of the game object
		/// </summary>
		/// <param name="newPosition">The new position</param>
		void setPosition(const glm::vec3 newPosition);

		/// <summary>
		/// Get the type of the game object
		/// </summary>
		/// <returns>The type</returns>
		std::string getType() const;

		/// <summary>
		/// Get the id of the game object
		/// </summary>
		/// <returns>The id</returns>
		long int getID() const;

		/// <summary>
		/// Set the type of the object
		/// </summary>
		/// <param name="newType">The new type</param>
		void setType(const std::string newType);

		/// <summary>
		/// Makes the physics computations to update things like position, velocity, acceleration, etc.
		/// </summary>
		/// <param name="deltaTime">The delta time of the frame</param>
		void UpdatePhysics(const double deltaTime);

		/// <summary>
		/// Turns the physics simulations on for the object
		/// </summary>
		void EnablePhysics();

		/// <summary>
		/// Turns the physics simulations off for the object
		/// </summary>
		void DisablePhysics();

		/// <summary>
		/// Set the movement type for the rigidbody
		/// Simulated - use physics to determine the position
		/// Function - the movement is based on a time-dependant function
		/// </summary>
		/// <param name="type">The type of movement</param>
		void MovementType(PhysicsConstants::Motion_Type type);

		/// <summary>
		/// Set the function that will be used for the function based movement
		/// </summary>
		/// <param name="f">The function that will be used. It will have a state, the total time and deltaTime as parameters</param>
		void MovementFunction(void (*f)(State& state, double time, double dt));

		/// <summary>
		/// Set the testure of the object
		/// </summary>
		/// <param name="texture"></param>
		void SetTexture(Texture2D& texture);

		/// <summary>
		/// Returns a reference to the rigidbody of the object
		/// </summary>
		/// <returns></returns>
		RigidBody& getRigidBody();
	};
}

