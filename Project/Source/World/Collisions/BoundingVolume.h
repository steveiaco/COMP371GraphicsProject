#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "../../ParsingHelper.h"


class BoundingSphere;
class BoundingBox;
class EmptyVolume;

enum Type {SPHERE, BOX};

class BoundingVolume {
public:
    BoundingVolume();
    virtual ~BoundingVolume() = default;
    virtual BoundingVolume* Clone() const = 0;

    virtual void SetPosition(glm::vec3 position);
    virtual void SetRotation(glm::vec3 axis, float angleDegrees);

    virtual glm::vec3 GetPosition() const		{ return mPosition; }
    virtual glm::vec3 GetRotationAxis() const	{ return mRotationAxis; }
    virtual float     GetRotationAngle() const	{ return mRotationAngleInDegrees; }
    virtual bool IsInVolume(BoundingVolume *volume) = 0;
    virtual std::string GetType() { return ""; };
    static BoundingVolume* InitializeVolume(const std::vector<ci_string>& token);
    static BoundingVolume* InitializeVolume();
    static bool SphereBoxCollision(BoundingSphere* sphere, BoundingBox* box);
    static bool SphereCollision(BoundingSphere* a, BoundingSphere* b);
    static bool BoxCollision(BoundingBox* a, BoundingBox* b);
    static bool CollisionWithEmpty(EmptyVolume* e, BoundingVolume* v) { return false; };
    static bool IsValid(BoundingVolume* volume);

protected:
    glm::vec3 mPosition;
    glm::vec3 mRotationAxis;
    float     mRotationAngleInDegrees;
};
