//
// bi_tools.h
//
// This is a collection of tools that make compiling on both platforms easier
// for my personal projects.

#pragma once
#include <string>
#include <stdio.h>

// Pauses the program in the command line in a way
// that can be simply implemented cross-platform
//
void biPause(){
  std::cout << "Press Enter to continue...\n";
  std::cin.ignore();
}

// Formats the given path for windows or apple and
// returns the result.
//
// E.g.
// Windows:    "myFile.txt"
// Mac:        "/myfile.txt"
//
// const char biPath(char* &filename){
//     #ifdef __APPLE__
//         return filename;
//     #elif defined _WIN32 || defined _WIN64
//         return filename;
//     #else
//     #endif
// }