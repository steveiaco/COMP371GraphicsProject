#pragma once

#include <glm/glm.hpp>
#include "BoundingVolume.h"

class BoundingBox;

class BoundingSphere : public BoundingVolume {
public:
    BoundingSphere();
    BoundingSphere(const BoundingSphere* sphere);
    BoundingSphere(glm::vec3 position, float radius);
    ~BoundingSphere();
    void SetRadius(float radius);

    virtual BoundingSphere* Clone() const;
    std::string GetType() { return "Sphere"; };

    bool IsInVolume(BoundingVolume *volume);
    float GetRadius() const { return mRadius; }
    float GetScaledRadius() const;
private:
    float mRadius;
};
