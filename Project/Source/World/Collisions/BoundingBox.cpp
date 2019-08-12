#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "EmptyVolume.h"

BoundingBox::BoundingBox() : BoundingVolume(),  mStrides(0.0f, 0.0f, 0.0f)
{
}

BoundingBox::BoundingBox(const BoundingBox *box) : BoundingBox()
{
    mPosition = box->mPosition;
    mRotationAngles = box->mRotationAngles;
    mStrides = box->mStrides;
    mScaling = box->mScaling;
}

BoundingBox* BoundingBox::Clone() const
{
    return new BoundingBox(this);
}

BoundingBox::~BoundingBox()
{
    delete this;
}

glm::vec3 BoundingBox::GetMax() const
{
    glm::vec3 scaledStride = mScaling * mStrides;
    return glm::vec3(BoundingVolume::GetPosition() + scaledStride);
}

glm::vec3 BoundingBox::GetMin() const
{
    glm::vec3 scaledStride = mScaling * mStrides;
    return glm::vec3(BoundingVolume::GetPosition() - scaledStride);
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
    else if (dynamic_cast<EmptyVolume*>(volume))
    {
        return BoundingVolume::CollisionWithEmpty(dynamic_cast<EmptyVolume*>(volume), this);
    }

    return false;
}
