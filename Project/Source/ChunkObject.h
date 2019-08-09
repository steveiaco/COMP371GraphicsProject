#pragma once

#include "ParsingHelper.h"
#include <istream>
#include <glm/glm.hpp>
#include <vector>
#include "TextureLoader.h"


//Defines a model, but holds no world information about said model.
class ChunkObject 
{
public:
	ChunkObject();
	virtual ~ChunkObject();

	void Load(ci_istringstream& iss);
	virtual bool ParseLine(const std::vector<ci_string>& token);

	inline glm::vec2 GetTemperatureRange() { return glm::vec2(minTemperature, maxTemperature); }
	inline glm::vec2 GetAltitudeRange() { return glm::vec2(minAltitude, maxAltitude); }
	inline glm::vec2 GetHumidityRange() { return glm::vec2(minHumidity, maxHumidity); }

	inline glm::vec3 GetMinScaling() { return minScaling; }
	inline glm::vec3 GetMaxScaling() { return maxScaling; }
	inline glm::vec3 GetMinRotationAngle() { return minRotation; }
	inline glm::vec3 GetMaxRotationAngle() { return maxRotation; }

private:


	struct Vertex 
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec4 color;
		glm::vec2 texture;
	};

	struct Face
	{
		std::vector<int> vertices;
		std::vector<int> textures;
		std::vector<int> normals;
	};

	void LoadOBJ(const char * path);
	std::vector<ChunkObject::Vertex> GenerateOrderedVertexList();
	std::vector<ChunkObject::Face> ConvertQuadToTris(Face quad);

	std::vector<std::string> split(const std::string & s, char delim);

	unsigned int mVAO;
	unsigned int mVBO;

	ci_string mName;

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

	int textureID;


	//Object generation parameters, used to clamp generation 
	glm::vec3 minScaling;
	glm::vec3 maxScaling;

	glm::vec3 minRotation;
	glm::vec3 maxRotation;
	
	//Biome parameters used to determine whether the object can spawn in a chunk
	float minTemperature;
	float maxTemperature;
	
	float minHumidity;
	float maxHumidity;
	
	float minAltitude;
	float maxAltitude;

	friend class ChunkObjectInstance;
};