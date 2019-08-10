#include <map>
#include "BoundingVolume.h"
#include "BoundingBox.h"
#include "BoundingSphere.h"

std::map<char, Type> supportedTypes = {
        {'S', SPHERE},
        {'B', BOX}
};

BoundingVolume::BoundingVolume() : mPosition(glm::vec3(0.0f, 0.0f, 0.0f)), mRotationAxis(glm::vec3(0.0f, 1.0f, 0.0f)), mRotationAngleInDegrees(0.0f) {}

BoundingVolume* BoundingVolume::InitializeVolume(const std::vector<ci_string>& token)
{
    char inputType = token[2].c_str()[0];
    if (supportedTypes.find(inputType) != supportedTypes.end() ) {
        Type type = supportedTypes[inputType];
        switch (type) {
            case SPHERE:
            {
                BoundingSphere *s = new BoundingSphere();
                s->SetRadius(static_cast<float>(atof(token[3].c_str())));
                return s;
            }
            case BOX:
            {
                BoundingBox* b = new BoundingBox();
                b->SetStrides(static_cast<float>(atof(token[3].c_str())), static_cast<float>(atof(token[4].c_str())), static_cast<float>(atof(token[5].c_str())));
                return b;
            }
            default:
                return nullptr;
        }
    }
    return nullptr;
}
void BoundingVolume::SetPosition(glm::vec3 position)
{
    mPosition = position;
}

void BoundingVolume::SetRotation(glm::vec3 axis, float angleDegrees)
{
    mRotationAxis = axis;
    mRotationAngleInDegrees = angleDegrees;
}

bool BoundingVolume::SphereBoxCollision(BoundingSphere *sphere, BoundingBox *box)
{
    glm::vec3 max = box->GetMax();
    glm::vec3 min = box->GetMin();
    glm::vec3 spherePos = sphere->GetPosition();

    float x = glm::max(min.x, glm::min(spherePos.x, max.x));
    float y = glm::max(min.y, glm::min(spherePos.y, max.y));
    float z = glm::max(min.z, glm::min(spherePos.z, max.z));

    double distance = sqrt((x - spherePos.x) * (x - spherePos.x) +
                           (y - spherePos.y) * (y - spherePos.y) +
                           (z - spherePos.z) * (z - spherePos.z));

    return distance < sphere->GetRadius();
}

bool BoundingVolume::SphereCollision(BoundingSphere *a, BoundingSphere *b)
{
    double distanceBetweenSpheres = sqrt(pow(a->GetPosition().x - b->GetPosition().x, 2)
                                         + pow(a->GetPosition().y - b->GetPosition().y, 2)
                                         + pow(a->GetPosition().z - b->GetPosition().z, 2));

    return distanceBetweenSpheres < (a->GetRadius() + b->GetRadius());
}

bool BoundingVolume::BoxCollision(BoundingBox *a, BoundingBox *b)
{
    glm::vec3 maxA = a->GetMax();
    glm::vec3 minA = a->GetMin();

    glm::vec3 maxB = b->GetMax();
    glm::vec3 minB = b->GetMin();

    return (minA.x <= maxB.x && maxA.x >= minB.x) &&
           (minA.y <= maxB.y && maxA.y >= minB.y) &&
           (minA.z <= maxB.z && maxA.z >= minB.z);
}