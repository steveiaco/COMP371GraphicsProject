#pragma once

#include "ParsingHelper.h"

#include <vector>

#include <glm/glm.hpp>

class LightSource
{
public:
	LightSource();
	~LightSource();

	void Load(ci_istringstream& iss);
	bool ParseLine(const std::vector<ci_string> &token);

	glm::vec4 GetPosition() { return mPosition;}
	glm::vec3 GetColor() { return mColor; }
	glm::vec3 GetAttenuation() { return mAttenuation; }

private:
	glm::vec4 mPosition = glm::vec4(0.0, -1.0, 0.0, 0.0); //Vector for directional lighting, point for point lighting
	glm::vec3 mColor = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 mAttenuation = glm::vec3(1.0, 0.0, 0.0);
};