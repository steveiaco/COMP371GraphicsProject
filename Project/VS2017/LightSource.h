#pragma once

#include "ParsingHelper.h"

#include <glm/glm.hpp>

class LightSource
{
public:
	LightSource();
	LightSource(glm::vec4 position, glm::vec3 color);
	virtual ~LightSource();

	void Load(ci_istringstream& iss);
	virtual bool ParseLine(const std::vector<ci_string> &token) = 0;

	glm::vec4 GetPosition() { return mPosition;}
	glm::vec3 GetColor() { return mColor; }

private:
	glm::vec4 mPosition = glm::vec4(0.0, -1.0, 0.0, 0.0); //Vector for directional lighting, point for point lighting
	glm::vec3 mColor = glm::vec3(1.0, 1.0, 1.0);
};