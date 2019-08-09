#pragma once

#include <glm/glm.hpp>

class BoundingVolume {
public:
    BoundingVolume();
    ~BoundingVolume() = default;

    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 axis, float angleDegrees);

    glm::vec3 GetPosition() const		{ return mPosition; }
    glm::vec3 GetRotationAxis() const	{ return mRotationAxis; }
    float     GetRotationAngle() const	{ return mRotationAngleInDegrees; }
    virtual bool isInVolume(BoundingVolume *volume) = 0;
private:
    glm::vec3 mPosition;
    glm::vec3 mRotationAxis;
    float     mRotationAngleInDegrees;
};
