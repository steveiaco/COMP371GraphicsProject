#include "ObjectModel.h"

ObjectModel::ObjectModel() : Model()
{
	//Attempt to locate file, if file is not located, load the default model
}

ObjectModel::~ObjectModel()
{
}

void ObjectModel::Update(float dt)
{
}

void ObjectModel::Draw()
{
}

bool ObjectModel::ParseLine(const std::vector<ci_string>& token)
{
	if (token.empty())
	{
		return true;
	}
	else if (token[0] == "density") 
	{

	}
	else if (token[0] == "path") 
	{
		path = 
	}
}

Vertex* ObjectModel::LoadVertices(const char * path)
{
	// Using case-insensitive strings and streams for easier parsing
	ci_ifstream input;
	input.open(path, ios::in);

	// Invalid file
	if (input.fail())
	{
		fprintf(stderr, "Error loading file: %s\n", path);
		getchar();
		exit(-1);
	}
}
