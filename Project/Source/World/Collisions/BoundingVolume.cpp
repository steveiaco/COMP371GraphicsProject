#include <map>
#include <cstring>
#include "BoundingVolume.h"
#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "EmptyVolume.h"

std::map<char, Type> supportedTypes = {
        {'S', SPHERE},
        {'B', BOX},
        {'E', EMPTY}
};

BoundingVolume::BoundingVolume() : mPosition(glm::vec3(0.0f, 0.0f, 0.0f)), mRotationAngles(glm::vec3(0.0f, 0.0f, 0.0f)), mScaling(1.0f, 1.0f, 1.0f)
{

}

BoundingVolume* BoundingVolume::InitializeVolume(const std::vector<ci_string>& token)
{
    char inputType = token[2].c_str()[0];
    if (supportedTypes.find(inputType) != supportedTypes.end() ) {
        Type type = supportedTypes[inputType];
        switch (type) {
            case SPHERE:
            {
                if (token.size() != 4)
                {
                    printf("The sphere collider requires 'S' and the single radius as parameters\nDiscarding collider...\n");
                    return new EmptyVolume();;
                }
                BoundingSphere *s = new BoundingSphere();
                s->SetRadius(static_cast<float>(atof(token[3].c_str())));
                return s;
            }
            case BOX:
            {
                if (token.size() != 6)
                {
                    printf("The box collider requires 'B' and the 3 stride values (on x, y, z) as parameters\nDiscarding collider...\n");
                    return new EmptyVolume();;
                }
                BoundingBox* b = new BoundingBox();
                b->SetStrides(static_cast<float>(atof(token[3].c_str())), static_cast<float>(atof(token[4].c_str())), static_cast<float>(atof(token[5].c_str())));
                return b;
            }
            default:
                return new EmptyVolume();
        }
    }
    return new EmptyVolume();
}
BoundingVolume* BoundingVolume::InitializeVolume()
{
    return new EmptyVolume();
}

void BoundingVolume::SetPosition(glm::vec3 position)
{
    mPosition = position;
}

void BoundingVolume::SetRotation(glm::vec3 angles)
{
    mRotationAngles = angles;
}

void BoundingVolume::SetScaling(glm::vec3 scale)
{
    mScaling = scale;
}

bool BoundingVolume::SphereBoxCollision(BoundingSphere *sphere, BoundingBox *box)
{
    glm::vec3 max = box->GetMax();
    glm::vec3 min = box->GetMin();
    glm::vec3 spherePos = sphere->GetPosition();

    float x = glm::max(min.x, glm::min(spherePos.x, max.x));
    float y = glm::max(min.y, glm::min(spherePos.y, max.y));
    float z = glm::max(min.z, glm::min(spherePos.z, max.z));

    double distanceSquared = ((x - spherePos.x) * (x - spherePos.x) +
                           (y - spherePos.y) * (y - spherePos.y) +
                           (z - spherePos.z) * (z - spherePos.z));

    return sqrt(distanceSquared) < sphere->GetScaledRadius();
}

bool BoundingVolume::SphereCollision(BoundingSphere *a, BoundingSphere *b)
{
    double distanceBetweenSpheresSquared = (a->GetPosition().x - b->GetPosition().x) * (a->GetPosition().x - b->GetPosition().x)
                                         + (a->GetPosition().y - b->GetPosition().y) * (a->GetPosition().y - b->GetPosition().y)
                                         + (a->GetPosition().z - b->GetPosition().z) * (a->GetPosition().z - b->GetPosition().z);

    return sqrt(distanceBetweenSpheresSquared) < (a->GetScaledRadius() + b->GetScaledRadius());
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

bool BoundingVolume::IsValid(BoundingVolume *volume) {
    if (volume == nullptr)
    {
        return false;
    }

    if (!volume->GetType().empty())
    {
        return (volume->GetType().c_str()[0] != 'E');
    }
    return false;
}