#pragma once

#include <tuple>
#include <vector>
#include <string>

struct museVertTexDataLine
{
	std::tuple<float, float, float> vertex;
	std::tuple<float, float> uv;
};

struct screenDimensions
{
	int session_screen_width;
	int session_screen_height;
};

std::vector<museVertTexDataLine> parseObj(std::string filename);

std::vector<std::string> split(std::string input, char splitter);
