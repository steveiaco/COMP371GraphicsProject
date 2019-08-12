#include "BSplineCamera.h"
#include "EventManager.h"
#include "World.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>
#include <algorithm>


using namespace glm;

BSplineCamera::BSplineCamera(BSpline* spline, float speed)
    : Camera(), mpSpline(spline), mSpeed(speed), mSplineParameterT(0.0f)
{
    assert(spline != nullptr);
    mPosition = mpSpline->GetPosition(mSplineParameterT);
    
    mLookAt = vec3(0.0f, 0.0f, -1.0f);
    mUp = vec3(0.0f, 1.0f, 0.0f);
}

BSplineCamera::~BSplineCamera()
{
}

void BSplineCamera::Update(float dt)
{
    mLookAt = mpSpline->GetTangent(mSplineParameterT);

    float tangentMagnitude = length(mLookAt);
    mLookAt = normalize(mLookAt);

    mSplineParameterT += mSpeed * dt / tangentMagnitude;
    mPosition = mpSpline->GetPosition(mSplineParameterT);
}

glm::mat4 BSplineCamera::GetViewMatrix() const
{
    return glm::lookAt(mPosition, mPosition + mLookAt, mUp);
}
