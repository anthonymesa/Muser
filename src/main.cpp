#include <iostream>
#include <vector>
#include <sstream>
#include <cstring>
#include "MuseHandler.hpp"
#include "AudioHandler.hpp"
#include "Mantis.hpp"
#include "Main.hpp"

int main(int argc, char *argv[])
{
	if(argc > 1){
		// This skips the file name argument
		argv++;
		argc--;
		SetOptions(argc, argv);
	}

	unsigned int muse_1 = MuseHandler::NewMuse("muse_1");

	MuseHandler::RenderToImage(muse_1, 500, 500);

	MuseHandler::CleanUp();
}

/*
 * Sets running options
 *
 * Options:
 * 		-m, --mantis: display mantis
 */
void SetOptions(int argc, char *argv[]){
	while(argc != 0){

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
