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

    bool IsInVolume(BoundingVolume *volume);
    float GetRadius() const { return mRadius; }
private:
    float mRadius;
};
