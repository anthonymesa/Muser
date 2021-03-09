#pragma once

#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <filesystem>
#include <iterator>

class Mantis{
public:

	Mantis(std::string name);
	
	template<typename T> void Log(T const input);
	template<typename T> std::ostringstream Print(T const input);

	static void Init(char argv[]);
	static void Clean();

private:

	static std::string MANTIS_LOGS_FOLDER;
	std::string name;
	std::ofstream log_file;
};

typedef std::vector<int> vec_i;
typedef std::vector<float> vec_f;
typedef std::vector<unsigned int> vec_ui;
typedef std::tuple<int, int> tup_ii;
typedef std::tuple<float, float> tup_ff;
typedef std::tuple<float, float, float> tup_fff;
typedef std::vector<std::vector<unsigned int>> vec_vec_ui;
typedef std::set<std::tuple<int, int>> set_tup_ii;
std::string Mantis::MANTIS_LOGS_FOLDER = "";

Mantis::Mantis(std::string name)
{
	this->name = name;
}

void Mantis::Init(char argv[])
{
	std::string temp_arg(argv);
	std::string temp_path(std::filesystem::current_path());
	MANTIS_LOGS_FOLDER = temp_path.append(temp_arg);
	Mantis::Clean();
}

void Mantis::Clean()
{
	std::filesystem::path dir = MANTIS_LOGS_FOLDER;
	for (const auto& entry : std::filesystem::directory_iterator(dir))
		if (entry != dir) {
			std::filesystem::remove_all(entry.path());
		}

	//int success = std::filesystem::remove_all(dir);
	//std::cout << success << std::endl;
}

//ATTENTION: This overload takes a line_length for formatting the print of
// a really long vector, though I removed that functionality.
template<typename T> std::ostringstream Mantis::Print(T const input)
{
	std::ostringstream oss;

	if constexpr (std::is_same_v<T, int>)
	{
		oss << std::to_string(input);
	}
	else if constexpr (std::is_same_v<T, const char*>)
	{
		std::string temp_input(input);
		oss << temp_input;
	}
	else if constexpr (std::is_same_v<T, std::string>)
	{
		oss << input;
	}
	else if constexpr (std::is_same_v< T, vec_i>)
	{
		if (!input.empty())
		{
			std::copy(input.begin(), input.end() - 1, std::ostream_iterator<int>(oss, " "));
			oss << input.back();
		}
	}
	else if constexpr (std::is_same_v< T, std::vector<float>>)
	{
		if (!input.empty())
		{
			std::copy(input.begin(), input.end() - 1, std::ostream_iterator<float>(oss, " "));
			oss << input.back();
		}
	}
	else if constexpr (std::is_same_v< T, vec_ui>)
	{
		if (!input.empty())
		{
			std::copy(input.begin(), input.end() - 1, std::ostream_iterator<unsigned int>(oss, " "));
			oss << input.back();
		}
	}
	else if constexpr (std::is_same_v<T, tup_ii>)
	{
		std::string temp_string(std::to_string(std::get<0>(input)) + " "
			+ std::to_string(std::get<1>(input)));
		oss << temp_string;
	}
	else if constexpr (std::is_same_v<T, tup_ff>)
	{
		std::string temp_string(std::to_string(std::get<0>(input)) + " "
			+ std::to_string(std::get<1>(input)));
		oss << temp_string;
	}
	else if constexpr (std::is_same_v<T, tup_fff>)
	{
		std::string temp_string(std::to_string(std::get<0>(input)) + " "
			+ std::to_string(std::get<1>(input)) + " " +
			+ std::to_string(std::get<2>(input)));
		oss << temp_string;
	}
	else if constexpr (std::is_same_v<T, vec_vec_ui>)
	{
		for (auto each : input)
		{
			oss << Mantis::Print(each).str() << std::endl;
		}
	}
	else if constexpr (std::is_same_v<T, set_tup_ii>)
	{
		for (auto each : input)
		{
			oss << Mantis::Print(each).str() << std::endl;
		}
	}

	return oss;
}

//ATTENTION: upon retrospect, I may not even need overloading...
template<typename T> void Mantis::Log(T const input)
{
	std::string filename_string = (MANTIS_LOGS_FOLDER + "/" + name + ".txt");
	char* filename_char = const_cast<char*>(filename_string.c_str());
	log_file.open(filename_char, std::ofstream::out | std::ofstream::app);

	log_file << Mantis::Print(input).str() << std::endl;
	log_file.close();
}
