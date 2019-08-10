#pragma once

#include <glm/glm.hpp>
#include "BoundingVolume.h"

class BoundingBox;

class BoundingSphere : public BoundingVolume {
public:
    BoundingSphere();
    ~BoundingSphere() = default;
    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 axis, float angleDegrees);
    void SetRadius(float radius);

    glm::vec3 GetPosition() const		{ return mPosition; }
    glm::vec3 GetRotationAxis() const	{ return mRotationAxis; }
    float     GetRotationAngle() const	{ return mRotationAngleInDegrees; }
    bool IsInVolume(BoundingVolume *volume);
    float GetRadius() const { return mRadius; }
private:
    glm::vec3 mPosition;
    glm::vec3 mRotationAxis;
    float     mRotationAngleInDegrees;
    float mRadius;
};
