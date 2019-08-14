#pragma once

#include <glm/glm.hpp>
#include "BoundingVolume.h"

// The BoundingBox is Axis-Aligned and the
// position is assumed to be at the bottom center
class BoundingBox : public BoundingVolume {
public:
    BoundingBox();
    BoundingBox(const BoundingBox* box);
    ~BoundingBox();
    void SetStrides(float x, float y, float z);
    void SetStrides(glm::vec3 strides);

    virtual BoundingBox* Clone() const;
    std::string GetType() { return "B"; };

    glm::vec3 GetStrides() const		{ return mStrides; }
    glm::vec3 GetMax() const;
    glm::vec3 GetMin() const;

    bool IsInVolume(BoundingVolume *volume);
private:
    glm::vec3 mStrides;
};