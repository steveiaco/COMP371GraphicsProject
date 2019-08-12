#pragma once

#include "ParsingHelper.h"
#include "Model.h"
#include <vector>

#include <glm/glm.hpp>


class BSpline : public Model
{
public:
    
	BSpline();
    BSpline(glm::vec2 origin, glm::vec2 size, glm::vec2 heightLimit, unsigned int count);
    virtual ~BSpline();

	void CreateVertexBuffer();

    virtual void Update(float dt);
    virtual void Draw();

    void AddControlPoint(glm::vec3 waypoint);
    void ClearControlPoints();

	// t from 0 to 1 is the first segment, from 1 to 2 is the second, etc...
    // this is the more general BSpline equation, 4 control points, 0 <= t <= 1
	glm::vec3 GetPosition(float t) const;
	static glm::vec3 GetPosition(float t, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& p4);

    glm::vec3 GetTangent(float t) const;
    static glm::vec3 GetTangent(float t, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& p4);

protected:
    virtual bool ParseLine(const std::vector<ci_string> &token);

private:
	// The vertex format could be different for different types of models
	struct Vertex
	{
		glm::vec3 position;
	};

	unsigned int mVAO;
	unsigned int mVBO;

    // Control points used by the B-Spline equation
    std::vector<glm::vec3> mControlPoints;

    // Sample points along the curve, used for drawing
    std::vector<glm::vec3> mSamplePoints;

    void GenerateSamplePoints();
};
