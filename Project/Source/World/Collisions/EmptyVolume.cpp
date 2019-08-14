#include "EmptyVolume.h"

EmptyVolume::EmptyVolume() : BoundingVolume()
{
}

EmptyVolume::EmptyVolume(const EmptyVolume *empty) : EmptyVolume()
{
    mPosition = empty->mPosition;
    mRotationAngles = empty->mRotationAngles;
    mScaling = empty->mScaling;
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
