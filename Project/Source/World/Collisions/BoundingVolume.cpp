#include "BoundingVolume.h"

BoundingVolume::BoundingVolume() : mPosition(glm::vec3(0.0f, 0.0f, 0.0f)), mRotationAxis(glm::vec3(0.0f, 1.0f, 0.0f)), mRotationAngleInDegrees(0.0f) {}

void BoundingVolume::SetPosition(glm::vec3 position)
{
    mPosition = position;
}

void BoundingVolume::SetRotation(glm::vec3 axis, float angleDegrees)
{
    mRotationAxis = axis;
    mRotationAngleInDegrees = angleDegrees;
}