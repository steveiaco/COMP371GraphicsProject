#include "EmptyVolume.h"

EmptyVolume::EmptyVolume() : BoundingVolume()
{
}

EmptyVolume::EmptyVolume(const EmptyVolume *empty)
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
    return false;
}
