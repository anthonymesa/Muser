#include <iostream>
#include <vector>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <string>
#include "MuseHandler.hpp"
#include "AudioHandler.hpp"
#include "Mantis.hpp"
#include "Muser.hpp"

int main(int argc, char *argv[])
{
	bool PROGRAM_SHOULD_RUN = true;

	if(argc > 1){
		// This skips the file name argument
		argv++;
		argc--;
		SetOptions(argc, argv);
	}

	std::cout << "Welcome to Muser:" << std::endl;

	while(PROGRAM_SHOULD_RUN){
		std::string input;
		std::cout << "~ ";
		std::getline(std::cin, input);

		std::vector<std::string> command = SplitString(input, ' ');

		if (command[0].compare("nm") == 0)
		{
			if(command.size() == 2)
			{
				unsigned int new_muse_index = MuseHandler::NewMuse(command[1].c_str());
				std::cout << "muse created - " << new_muse_index << std::endl;
			}
		}
		else if (command[0].compare("rtoi") == 0)
		{
			if(command.size() == 4)
			{
				MuseHandler::RenderToImage(stoi(command[1]), stoi(command[2]), stoi(command[3]));
			}
		}
		else if (command[0].compare("rm") == 0)
		{
			if(command.size() == 2)
			{	
				std::cout << "function not ready yet" << std::endl;
			}
		}
		else if (command[0].compare("exit") == 0){
			PROGRAM_SHOULD_RUN = false;
		}
	}
	MuseHandler::CleanUp();
	return 0;
}

/*
 * Sets running options
 *
 * Options:
 * 		-m, --mantis: display mantis
 */
void SetOptions(int argc, char *argv[]){
	while(argc > 0){

		/* Runs mantis debugger using specified
		 * logs folder
		 */
		if( (strcmp(argv[0], "-m") == 0) || (strcmp(argv[0], "--mantis") == 0) ){
			Mantis::Init(argv[1]);
			argv = argv + 2;
			argc = argc - 2;
		}
	}
}

// This is the same function used in Object handler. best way to implement it rn.
std::vector<std::string> SplitString(const std::string& line, const char& delimiter)
{
	std::vector<std::string> result;
	int element_index = 0;
	for (size_t i = 0; i < line.size(); i++)
	{
		if (line[i] == delimiter)
		{
			result.push_back(line.substr(element_index, i - element_index));
			element_index = i + 1;
		}
	}
	result.push_back(line.substr(element_index));
	return result;
}