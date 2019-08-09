#pragma once

#include <glm/glm.hpp>
#include "BoundingVolume.h"
#include "BoundingSphere.h"

class BoundingBox : public BoundingVolume {
public:
    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 axis, float angleDegrees);

    glm::vec3 GetPosition() const		{ return mPosition; }
    glm::vec3 GetRotationAxis() const	{ return mRotationAxis; }
    float     GetRotationAngle() const	{ return mRotationAngleInDegrees; }
    glm::vec3 GetStrides() const		{ return mStrides; }

    bool isInVolume(BoundingVolume *volume);
private:
    glm::vec3 mPosition;
    glm::vec3 mRotationAxis;
    float     mRotationAngleInDegrees;
    glm::vec3 mStrides;

    bool isInVolume(BoundingBox *box);
    bool isInVolume(BoundingSphere *sphere);
};