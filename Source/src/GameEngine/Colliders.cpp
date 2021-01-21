#include "Colliders.hpp"

glm::vec3 GameEngine::Collider::getPosition() const
{
    return glm::vec3(position);
}

void GameEngine::Collider::setPosition(const glm::vec3& pos)
{
    position = pos;
}

bool GameEngine::Collider::isAffectingPhysics() const
{
    return _affectsPhysics;
}

void GameEngine::Collider::affectsPhysics(const bool aff)
{
    _affectsPhysics = aff;
}

long int GameEngine::Collider::getID() const
{
    return gameObjectID;
}

GameEngine::ColliderType GameEngine::Collider::getColliderType() const
{
    return type;
}

GameEngine::Collider::Collider(const Collider& other)
{
    gameObjectID = other.gameObjectID;
    type = other.type;
    _affectsPhysics = other._affectsPhysics;
    radius = other.radius;
    position = other.position;
    dimensions = other.dimensions;
}

GameEngine::Collider::Collider(const long int id, const glm::vec3& pos, const glm::vec3& dim)
{
    gameObjectID = id;
    _affectsPhysics = false;
    position = position;
    dimensions = dim;
    radius = 0;
    type = ColliderType::BoxCollider;
}

glm::vec3 GameEngine::Collider::getDimensions() const
{
    return glm::vec3(dimensions);
}

void GameEngine::Collider::setDimensions(const glm::vec3& dim)
{
    dimensions = dim;
}

GameEngine::Collider::Collider(const long int id, const glm::vec3& pos, const double rad)
{
    gameObjectID = id;
    _affectsPhysics = false;
    position = pos;
    radius = rad;
    dimensions = glm::vec3(1);
    type = ColliderType::SphereCollider;
}

double GameEngine::Collider::getRadius() const
{
    return radius;
}

void GameEngine::Collider::setRadius(const double rad)
{
    radius = rad;
}