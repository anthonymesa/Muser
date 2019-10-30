#include <ObjParser.h>

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <string>

using namespace std;

using namespace std::string_literals;

/*
 *  Splits a string given a character to split with.
 *
 *  @param input String to splits
 *  @param splitter Character to split with
 *  @return result An array of String objects
 */

vector<string> split(string input, char splitter)
{
	vector<string> result;
	int start = 0;
	for (size_t i = 0; i < input.size(); i++)
	{
		if (input[i] == splitter)
		{
			result.push_back(input.substr(start, i - start));
			start = i + 1;
		}
	}
	result.push_back(input.substr(start));
	return result;
}

/*
 *  Takes in a local .obj file and parses the data into a set of Vector arrays
 *
 *  @params filename Path to local .obj file
 *  @return lines 
 */

std::vector<museVertTexDataLine> parseObj(string filename) {
	// load path
	ifstream muse(filename);

	// if path doesnt exist, throw exception
	if (!muse.good())
	{
		throw std::runtime_error("There is no .obj file with name " + filename + " in resources folder");
	}

	cout << "\n" << "Opened File..." << "\n";

	// declare vertex and uv vector arrays
	vector<tuple<float, float, float>> verticies;
	vector<tuple<float, float>> uvs;
	string line;

	bool v = false, vt = false, f = false;
	float x, y, z;
	unordered_map<int, int> pointerMap;

	// for each line in file parse accordingly
	while (getline(muse, line))
	{
		// if the parsed line begins in v, vt, or f...
		if (line.find("v ") != string::npos)
		{
			if (v == false) {
				cout << "Reading all Verticies...\n";
				v = true;
			}
			// uses split function, removes the first value, and then pushes the three values to the verticies Vector
			vector<string> vSplit = split(line, ' ');
			vector<string> vShift(vSplit.begin() + 1, vSplit.end());
			x = (float)atof(vShift[0].c_str());
			y = (float)atof(vShift[1].c_str());
			z = (float)atof(vShift[2].c_str());
			verticies.push_back(make_tuple(x, y, z));
		}
		else if (line.find("vt ") != string::npos)
		{
			if (vt == false) {
				cout << "Reading all Texture Coordinates...\n";
				vt = true;
			}
			vector<string> vtSplit = split(line, ' ');
			vector<string> vtShift(vtSplit.begin() + 1, vtSplit.end());
			x = (float)atof(vtShift[0].c_str());
			y = (float)atof(vtShift[1].c_str());
			uvs.push_back(make_tuple(x, y));
		}
		else if (line.find("f ") != string::npos)
		{
			if (f == false) {
				cout << "Reading all Faces...\n";
				f = true;
			}
			vector<string> pointerList = split(line, ' ');
			vector<string> pointerListShift(pointerList.begin() + 1, pointerList.end());
			for (string& each : pointerListShift)
			{
				// for each line in pointerlistshift, split each element into pieces and assign to a map
				vector<string> pointer = split(each, '/');
				vector<string> pointerShift(pointer.begin(), pointer.begin() + 2);
				int key = atol(pointerShift[0].c_str());
				int value = atol(pointerShift[1].c_str());
				pointerMap[key - 1] = value - 1;
			}
		}
	}

	// sets variable to display the amount of vertex/uv's processed in file
	int mapCount = distance(pointerMap.begin(), pointerMap.end());

	ofstream muserBase;
	muserBase.open("res/base.mus");

	// for each element in the verticies array, using the pointer map, find the corresponding element from the uvs array,
	// and set both objects to variables that are then passed to museVertTexDataLine
	std::vector<museVertTexDataLine> lines;
	for (size_t i = 0; i < verticies.size(); i++) {
		tuple<float, float, float> vertex = verticies[i];
		tuple<float, float> uv = uvs[pointerMap[i]];

		muserBase << std::get<0>(vertex) << " " << std::get<1>(vertex) << " " << std::get<2>(vertex) << " " << std::get<0>(uv) << " " << std::get<1>(uv) << "\n";

		museVertTexDataLine line;
		line.vertex = vertex;
		line.uv = uv;

		lines.push_back(line);
	}
	muserBase.close();
	cout << "\n" << "Using map of " << mapCount << " entries, created new struct with " << lines.size() << " entries" << "\n";
	return lines;
}