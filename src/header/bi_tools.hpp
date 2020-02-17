//
// bi_tools.h
//
// This is a collection of tools that make compiling on both platforms easier
// for my personal projects.

#pragma once
#include <string>
#include <stdio.h>

const double bi_PI = 3.141592653589793;

void biPause(){
  std::cout << "Press Enter to continue...\n";
  std::cin.ignore();
}

void bug(const std::string &input)
{
  std::cout << input << std::endl;
}