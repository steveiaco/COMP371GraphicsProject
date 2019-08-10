#include "BoundingSphere.h"
#include "BoundingBox.h"

BoundingSphere::BoundingSphere() : BoundingVolume(), mPosition(0.0f, 0.0f, 0.0f), mRotationAngleInDegrees(0.0f), mRotationAxis(0.f, 0.0f, 0.0f), mRadius(0.0f)
{
}

void BoundingSphere::SetRadius(float radius)
{
    mRadius = radius;
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
    return false;
}
