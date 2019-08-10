#pragma once

#include "BoundingVolume.h"

class EmptyVolume : public BoundingVolume {
public:
    EmptyVolume();
    EmptyVolume(const EmptyVolume* empty);
    ~EmptyVolume();

    virtual EmptyVolume* Clone() const;

    bool IsInVolume(BoundingVolume *volume);
};