//
// bi_tools.h
//
// This is a collection of tools that make compiling on both platforms easier
// for my personal projects.

#pragma once
#include <string>
#include <stdio.h>

const double bi_PI = 3.141592653589793;

// Pauses the program in the command line in a way
// that can be simply implemented cross-platform
//
void biPause(){
  std::cout << "Press Enter to continue...\n";
  std::cin.ignore();
}