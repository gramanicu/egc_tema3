#include "CollisionManager.hpp"

std::vector<int> GameEngine::CollisionManager::getCollisions(const Collider& source, std::vector<Collider*> others)
{
    std::vector<int> collided;
    for (auto& other : others) {
        if (isCollision(source, *other)) {
            collided.push_back(other->getID());
        }
    }
    return collided;
}

GameEngine::CollisionManager::CollisionManager(){}

bool GameEngine::CollisionManager::isCollision(const Collider& a, const Collider& b)
{
    using namespace GameEngine;
    if (a.getColliderType() == b.getColliderType()) {
        if (a.getColliderType() == ColliderType::BoxCollider) {
            // -- Box - Box colliders --
            // AABB algorithm
            glm::vec3 firstMin, firstMax, secondMin, secondMax;
            glm::vec3 firstPos = a.getPosition();
            glm::vec3 secondPos = b.getPosition();
            glm::vec3 firstDim = a.getDimensions();
            glm::vec3 secondDim = b.getDimensions();

            // Compute the bounds
            firstMin = firstPos - firstDim * 0.5f;
            secondMin = secondPos - secondDim * 0.5f;
            firstMax = firstPos + firstDim * 0.5f;
            secondMax = firstPos + secondDim * 0.5f;

            // Check for the collision
            return ((firstMin.x <= secondMin.x && firstMax.x >= secondMax.x) &&
                (firstMin.y <= secondMin.y && firstMax.y >= secondMax.y) &&
                (firstMin.z <= secondMin.z && firstMax.z >= secondMax.z));
        }
        else {
            // -- Sphere - Sphere colliders --
            // Sphere collision detection algorithm
            glm::vec3 firstPos = a.getPosition();
            glm::vec3 secondPos = b.getPosition();
            double firstRad = a.getRadius();
            double secondRad = b.getRadius();

            // Compute the distance between the two sphere centers
            double distance = std::sqrt((firstPos.x - secondPos.x) * (firstPos.x - secondPos.x) +
                (firstPos.y - secondPos.y) * (firstPos.y - secondPos.y) +
                (firstPos.z - secondPos.z) * (firstPos.z - secondPos.z));

            return distance < (firstRad + secondRad);
        }
    }
    else {
        if (a.getColliderType() == ColliderType::BoxCollider) {
            // Sphere - AABB collision detection algorithm
            glm::vec3 boxPos = a.getPosition();
            glm::vec3 boxDim = a.getDimensions();
            glm::vec3 spherePosition = b.getPosition();

            // Compute the box bounds
            glm::vec3 min = boxPos - boxDim * 0.5f;
            glm::vec3 max = boxPos + boxDim * 0.5f;

            // Get the box closest point to the sphere
            glm::vec3 point = glm::vec3(std::max(min.x, std::min(spherePosition.x, max.x)),
                std::max(min.y, std::min(spherePosition.y, max.y)),
                std::max(min.z, std::min(spherePosition.z, max.z)));


            // Check if the point is inside the sphere
            float distance = std::sqrt((point.x - spherePosition.x) * (point.x - spherePosition.x) +
                (point.y - spherePosition.y) * (point.y - spherePosition.y) +
                (point.z - spherePosition.z) * (point.z - spherePosition.z));

            return distance < b.getRadius();
        }
        else {
            // Sphere - AABB collision detection algorithm
            glm::vec3 boxPos = b.getPosition();
            glm::vec3 boxDim = b.getDimensions();
            glm::vec3 spherePosition = a.getPosition();

            // Compute the box bounds
            glm::vec3 min = boxPos - boxDim * 0.5f;
            glm::vec3 max = boxPos + boxDim * 0.5f;

            // Get the box closest point to the sphere
            glm::vec3 point = glm::vec3(std::max(min.x, std::min(spherePosition.x, max.x)),
                std::max(min.y, std::min(spherePosition.y, max.y)),
                std::max(min.z, std::min(spherePosition.z, max.z)));


            // Check if the point is inside the sphere
            float distance = std::sqrt((point.x - spherePosition.x) * (point.x - spherePosition.x) +
                (point.y - spherePosition.y) * (point.y - spherePosition.y) +
                (point.z - spherePosition.z) * (point.z - spherePosition.z));

            return distance < a.getRadius();
        }
    }      
}