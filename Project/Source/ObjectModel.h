#pragma once

#include "Model.h"
#include <istream>
#include "TextureLoader.h"



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

	struct Vertex 
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec4 color;
		glm::vec2 texture;
	};

	//This stores the vertex indices which makeup each polygon in the model.
	struct Face 
	{
		std::vector<int> vertices;
		std::vector<int> textures;
		std::vector<int> normals;
	};

	unsigned int mVAO;
	unsigned int mVBO;

	//Stores a list of faces for the model, defined by vertex indices
	std::vector<Face> faces;

	//Stores a list of vertices for the model
	std::vector<glm::vec3> vertices;

	//Stores a list of normals for the model
	std::vector<glm::vec3> normals;

	//Stores a list of texture coordinates for the model
	std::vector<glm::vec3> textureCoordinates; //todo change this to a vec2

	//Defines the file path for the .obj file to be loaded.
	char * path;

	//Defines the file path to the model's texture
	char * texturePath;

	bool textured;

	int textureID;

	//todo place this in a more appropriate place...
	static std::vector<std::string> split(const std::string &s, char delim);
	
	std::vector<Vertex> GenerateOrderedVertexList();
	std::vector<Face> ConvertQuadToTris(Face quad);

};