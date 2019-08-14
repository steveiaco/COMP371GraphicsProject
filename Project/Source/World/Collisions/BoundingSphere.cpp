#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "EmptyVolume.h"

BoundingSphere::BoundingSphere() : BoundingVolume(), mRadius(0.0f)
{
}

BoundingSphere::BoundingSphere(const BoundingSphere *sphere) : BoundingSphere()
{
    mPosition = sphere->mPosition;
    mRotationAngles = sphere->mRotationAngles;
    mRadius = sphere->mRadius;
    mScaling = sphere->mScaling;
}

BoundingSphere* BoundingSphere::Clone() const
{
    return new BoundingSphere(this);
}

BoundingSphere::BoundingSphere(glm::vec3 position, float radius) : BoundingSphere()
{
    mPosition = position;
    mRadius = radius;
}

BoundingSphere::~BoundingSphere()
{
    delete this;
}

void BoundingSphere::SetRadius(float radius)
{
    mRadius = radius;
}

float BoundingSphere::GetScaledRadius() const
{
    // The scaling is assumed to be uniform
    return mRadius * mScaling.x;
}

bool BoundingSphere::IsInVolume(BoundingVolume *volume) {
    if(dynamic_cast<BoundingSphere*>(volume))
    {
        return BoundingVolume::SphereCollision(this, dynamic_cast<BoundingSphere*>(volume));
    }
    else if (dynamic_cast<BoundingBox*>(volume))
    {
        return BoundingVolume::SphereBoxCollision(this, dynamic_cast<BoundingBox*>(volume));
    }
    else if (dynamic_cast<EmptyVolume*>(volume))
    {
        return BoundingVolume::CollisionWithEmpty(dynamic_cast<EmptyVolume*>(volume), this);
    }
    return false;
}
