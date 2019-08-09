#include "BoundingBox.h"

bool BoundingBox::isInVolume(BoundingBox *box) {
    return false;
}

bool BoundingBox::isInVolume(BoundingVolume *volume) {
    if(dynamic_cast<BoundingBox*>(volume))
    {
        return isInVolume(dynamic_cast<BoundingBox*>(volume));
    }
    return false;
}