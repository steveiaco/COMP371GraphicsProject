#include "EmptyVolume.h"

EmptyVolume::EmptyVolume() : BoundingVolume()
{
}

EmptyVolume::EmptyVolume(const EmptyVolume *empty) : EmptyVolume()
{
    mPosition = empty->mPosition;
    mRotationAxis = empty->mRotationAxis;
    mRotationAngleInDegrees = empty->mRotationAngleInDegrees;
}

EmptyVolume* EmptyVolume::Clone() const
{
    return new EmptyVolume(this);
}

EmptyVolume::~EmptyVolume()
{
    delete this;
}

bool EmptyVolume::IsInVolume(BoundingVolume *volume) {
    return CollisionWithEmpty(this, volume);
}
