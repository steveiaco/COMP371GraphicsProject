#include "LightSource.h"

using namespace std;
using namespace glm;

LightSource::LightSource()
{
}

LightSource::~LightSource()
{
}

void LightSource::Load(ci_istringstream& iss)
{
	ci_string line;

	// Parse model line by line
	while (std::getline(iss, line))
	{
		// Splitting line into tokens
		ci_istringstream strstr(line);
		istream_iterator<ci_string, char, ci_char_traits> it(strstr);
		istream_iterator<ci_string, char, ci_char_traits> end;
		vector<ci_string> token(it, end);

		if (ParseLine(token) == false)
		{
			fprintf(stderr, "Error loading scene file... token:  %s!", token[0].c_str());
			getchar();
			exit(-1);
		}
	}
}

bool LightSource::ParseLine(const std::vector<ci_string> &token)
{
	if (token.empty() == false)
	{
		if (token[0].empty() == false && token[0][0] == '#')
		{
			return true;
		}
		else if (token[0] == "position")
		{
			assert(token.size() > 5);
			assert(token[1] == "=");

			mPosition.x = static_cast<float>(atof(token[2].c_str()));
			mPosition.y = static_cast<float>(atof(token[3].c_str()));
			mPosition.z = static_cast<float>(atof(token[4].c_str()));
			mPosition.w = static_cast<float>(atof(token[5].c_str()));
		}
		else if (token[0] == "color")
		{
			assert(token.size() > 4);
			assert(token[1] == "=");

			mColor.x = static_cast<float>(atof(token[2].c_str()));
			mColor.y = static_cast<float>(atof(token[3].c_str()));
			mColor.z = static_cast<float>(atof(token[4].c_str()));
		}
		else if (token[0] == "attenuation")
		{
			assert(token.size() > 4);
			assert(token[1] == "=");

			mAttenuation.x = static_cast<float>(atof(token[2].c_str()));
			mAttenuation.y = static_cast<float>(atof(token[3].c_str()));
			mAttenuation.z = static_cast<float>(atof(token[4].c_str()));
		}
		else
		{
			return false;
		}
	}

	return true;
}