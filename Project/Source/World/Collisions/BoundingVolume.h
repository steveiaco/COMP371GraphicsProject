#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "../../ParsingHelper.h"


class BoundingSphere;
class BoundingBox;

enum Type {SPHERE, BOX};

class BoundingVolume {
public:
    BoundingVolume();
    ~BoundingVolume() = default;

    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 axis, float angleDegrees);

    glm::vec3 GetPosition() const		{ return mPosition; }
    glm::vec3 GetRotationAxis() const	{ return mRotationAxis; }
    float     GetRotationAngle() const	{ return mRotationAngleInDegrees; }
    virtual bool IsInVolume(BoundingVolume *volume) = 0;
    static BoundingVolume* InitializeVolume(const std::vector<ci_string>& token);
    static bool SphereBoxCollision(BoundingSphere* sphere, BoundingBox* box);
    static bool SphereCollision(BoundingSphere* a, BoundingSphere* b);
    static bool BoxCollision(BoundingBox* a, BoundingBox* b);
private:
    glm::vec3 mPosition;
    glm::vec3 mRotationAxis;
    float     mRotationAngleInDegrees;
};
