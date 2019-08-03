#pragma once

#include "Model.h"
#include "ParsingHelper.h"

struct Vertex;
class ObjectModel : public Model 
{
public:
	ObjectModel();
	virtual ~ObjectModel();

	virtual void Update(float dt);
	virtual void Draw();

protected:
	virtual bool ParseLine(const std::vector<ci_string>& token);
	Vertex* LoadVertices(const char * path);
private:
	//TODO - We will have to add information about the texture map here
	struct Vertex 
	{
		glm::vec3 position;
		glm::vec3 normal;
	};

	unsigned int mVAO;
	unsigned int mVBO;
	unsigned int numberOfVertices;
	float density;
	char * path;

};