#pragma once

#include "Model.h"
#include <istream>
#include <regex>

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
		glm::vec3 color;
	};

	unsigned int mVAO;
	unsigned int mVBO;
	unsigned int numberOfVertices;
	unsigned int numberOfVertexNormals;
	unsigned int numberOfTextureCoordinates;

	//Stores a list of vertices for the model
	std::vector<Vertex> vertices;

	//Defines how densely placed the objects should be placed when randomly placed into the world.
	float density;

	//Defines the file path for the .obj file to be loaded.
	const char * path;

};