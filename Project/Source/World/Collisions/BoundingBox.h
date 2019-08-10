#pragma once

#include <glm/glm.hpp>
#include "BoundingVolume.h"
#include "BoundingSphere.h"

// The BoundingBox is Axis-Aligned
class BoundingBox : public BoundingVolume {
public:
    BoundingBox();
    ~BoundingBox() = default;
    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 axis, float angleDegrees);
    void SetStrides(float x, float y, float z);
    void SetStrides(glm::vec3 strides);

    glm::vec3 GetPosition() const		{ return mPosition; }
    glm::vec3 GetRotationAxis() const	{ return mRotationAxis; }
    float     GetRotationAngle() const	{ return mRotationAngleInDegrees; }
    glm::vec3 GetStrides() const		{ return mStrides; }
    glm::vec3 GetMax() const;
    glm::vec3 GetMin() const;

    bool IsInVolume(BoundingVolume *volume);
private:
    glm::vec3 mPosition{};
    glm::vec3 mRotationAxis{};
    float     mRotationAngleInDegrees{};
    glm::vec3 mStrides{};
};