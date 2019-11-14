/*
    bi_tools.h

    THIS IS A COLLECTION OF TOOLS THAT MAKE COMPILING ON BOTH PLATFORMS EASIER FOR 
    MY PERSONAL PROJECTS.
*/

#pragma once

#include <string>

/*
    PROVIDE VARIABLES AND FUNCTIONS BASED OFF OF DEVELOPMENT ENVIRONMENT.
*/

#ifdef __APPLE__

	char current_directory_char[256] = "/Volumes/MESA-USB/Projects/Programming/Muser/build/mac/Release";
    std::string current_directory_string = "/Volumes/MESA-USB/Projects/Programming/Muser/build/mac/Release";

#elif defined _WIN32 || defined _WIN64

    #include <windows.h>
    std::string current_directory = getCurrentPath();

    /*
        GET THE CURRENT (.EXE) DRIVE LETTER.
    */
    std::string getCurrentPath(){
        char pBuf[256]; 
        size_t len = sizeof(pBuf);

        // Should include what happens if getmodulefilename() returns -1
        int bytes = std::GetModuleFileName(NULL, pBuf, len);

        return convert(pBuf);
    }

#else
#endif


/*
    TAKE IN CHAR ARRAY AND RETURN CHAR ARRAY AS STRING.
*/
template<size_t N>
std::string convert(char const(&data)[N])
{
   return std::string(data, std::find(data, data + N, '\0'));
}