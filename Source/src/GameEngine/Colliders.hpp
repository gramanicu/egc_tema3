#pragma once

#include <Core/Engine.h>

namespace GameEngine {
    enum class ColliderType:char { BoxCollider, SphereCollider };
    
    /// <summary>
    /// Base collider class
    /// </summary>
    class Collider {
    private:
        long int gameObjectID;
        ColliderType type;
        bool _affectsPhysics;

        double radius;

        glm::vec3 position;
        glm::vec3 dimensions;

    public:
        // Copy Constructor
        Collider(const Collider& other);

        /// <summary>
        /// Create a new box collider, with a specific dimensions, linked to a gameObject with a specific id
        /// </summary>
        /// <param name="id">The id of the gameObject</param>
        /// <param name="position">The position of the box collider</param>
        /// <param name="dimension">The dimensions of the collider</param>
        Collider(const int long id, const glm::vec3& position, const glm::vec3& dimensions);

        /// <summary>
        /// Create a new sphere collider, with a specific radius, linked to a gameObject with a specific id
        /// </summary>
        /// <param name="id">The id of the gameObject</param>
        /// <param name="position">The position of the sphere collider</param>
        /// <param name="radius">The radius of the sphere</param>
        Collider(const int long id, const glm::vec3& position, const double radius);

        /// <summary>
        /// Get the position of this collider
        /// </summary>
        /// <returns>The position</returns>
        glm::vec3 getPosition() const;

        /// <summary>
        /// Set the position of this collider
        /// </summary>
        /// <param name="pos">The new position</param>
        void setPosition(const glm::vec3& pos);

        /// <summary>
        /// Check if this collider influences physics
        /// </summary>
        /// <returns>If it will influence</returns>
        bool isAffectingPhysics() const;

        /// <summary>
        /// Set whether this collider will affect physics
        /// </summary>
        /// <param name="aff">Bool value</param>
        void affectsPhysics(const bool aff);

        /// <summary>
        /// Get the id of the collider (and subsequently, the game object it is attached to)
        /// </summary>
        /// <returns>The id</returns>
        long int getID() const;

        /// <summary>
        /// Get the type of the collider
        /// </summary>
        /// <returns>The type</returns>
        ColliderType getColliderType() const;

        /// <summary>
        /// Get the dimensions/scale of the collider
        /// </summary>
        /// <returns>The dimensions (width, height, length)</returns>
        glm::vec3 getDimensions() const;

        /// <summary>
        /// Set the dimensions/scale of the collider
        /// (width, height, length)
        /// </summary>
        /// <param name="dim">The new dimensions</param>
        void setDimensions(const glm::vec3& dim);

        /// <summary>
        /// Get the radius of the sphere
        /// </summary>
        /// <returns>The radius</returns>
        double getRadius() const;

        /// <summary>
        /// Set the radius of the sphere
        /// </summary>
        /// <param name="rad">The new radius</param>
        void setRadius(const double radius);
    };
}