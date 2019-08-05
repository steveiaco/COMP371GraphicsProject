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

	//This stores the vertex indices which makeup each polygon in the model.
	struct Face 
	{
		std::vector<int> vertices;
		std::vector<int> textures;
		std::vector<int> normals;
	};

	//Enum which defines the type of placement used
	enum PlacementType 
	{

		procedural,
		manual
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
	std::vector<glm::vec3> textureCoordinates;

	//Defines how densely placed the objects should be placed when randomly placed into the world.
	float density;

	//Defines the file path for the .obj file to be loaded.
	const char * path;

	//Defines the file path to the model's texture
	const char * texturePath;

	//Defines the placement method of the object
	PlacementType placement;

	/* The following parameters/functions are used when using procedural placement */
	unsigned int numberOfObjects;
	std::vector<glm::vec3> mPositions;
	std::vector<glm::vec3> mScalings;
	std::vector<glm::vec3> mRotationAxes;
	std::vector<float> mRotationAnglesInDegrees;
	

	std::vector<glm::mat4> GetWorldMatrices() const;

	//todo place this in a more appropriate place...
	static std::vector<std::string> split(const std::string &s, char delim);
	
	std::vector<Vertex> generateOrderedVertexList();
	std::vector<Face> convertQuadToTris(Face quad);


};