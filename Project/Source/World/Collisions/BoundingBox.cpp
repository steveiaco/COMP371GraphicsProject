#include "BoundingBox.h"

BoundingBox::BoundingBox() : BoundingVolume(), mPosition(0.0f, 0.0f, 0.0f), mStrides(0.0f, 0.0f, 0.0f), mRotationAngleInDegrees(0), mRotationAxis(0.0f, 0.0f, 0.0f)
{

}

glm::vec3 BoundingBox::GetMax() const
{
    return glm::vec3(mPosition + mStrides);
}

glm::vec3 BoundingBox::GetMin() const
{
    return glm::vec3(mPosition - mStrides);
}

void BoundingBox::SetStrides(glm::vec3 strides)
{
    mStrides = strides;
}

void BoundingBox::SetStrides(float x, float y, float z)
{
    mStrides = glm::vec3(x, y, z);
}

bool BoundingBox::IsInVolume(BoundingVolume *volume) {
    if(dynamic_cast<BoundingBox*>(volume))
    {
        return BoundingVolume::BoxCollision(this, dynamic_cast<BoundingBox*>(volume));
    }
    else if (dynamic_cast<BoundingSphere*>(volume))
    {
        return BoundingVolume::SphereBoxCollision(dynamic_cast<BoundingSphere*>(volume), this);
    }

    return false;
}
