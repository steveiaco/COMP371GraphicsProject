#include "BoundingBox.h"

BoundingBox::BoundingBox() : BoundingVolume(),  mStrides(0.0f, 0.0f, 0.0f)
{
}

BoundingBox::BoundingBox(const BoundingBox *box)
{
    mPosition = box->mPosition;
    mRotationAxis = box->mRotationAxis;
    mRotationAngleInDegrees = box->mRotationAngleInDegrees;
    mStrides = box->mStrides;
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
    return glm::vec3(BoundingVolume::GetPosition() + mStrides);
}

glm::vec3 BoundingBox::GetMin() const
{
    return glm::vec3(BoundingVolume::GetPosition() - mStrides);
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
    printf("Check collision at (%f, %f, %f)\n", GetPosition().x, GetPosition().y, GetPosition().z);
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
