#include "BoundingSphere.h"

bool BoundingSphere::isInVolume(BoundingSphere *sphere) {
    double distanceBetweenSpheres = sqrt(pow(mPosition.x - sphere->mPosition.x, 2)
                                          + pow(mPosition.y - sphere->mPosition.y, 2)
                                          + pow(mPosition.z - sphere->mPosition.z, 2));

    return distanceBetweenSpheres < (mRadius + sphere->mRadius);
}

bool BoundingSphere::isInVolume(BoundingVolume *volume) {
    if(dynamic_cast<BoundingSphere*>(volume))
    {
        return isInVolume(dynamic_cast<BoundingSphere*>(volume));
    }
    return false;
}
