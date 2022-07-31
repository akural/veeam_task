#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include "MD5/MD5.h"
#include <fstream>
#include <limits>
#include <array>
#include <algorithm>
#include <boost/atomic.hpp>
#include <boost/thread.hpp>

std::streamsize GetFileSize(std::ifstream& file);

// This class manages all work with input files
class InputManager
{
public:
    InputManager(const std::string& fileName, char* buffer, int sizeBlock);
    virtual ~InputManager();

    void GetBlockInFile(int sizeBuffer); // Read data in block from input file

private:
    std::ifstream inputFile;
    int sizeBlock;                      // In Mb for reading
    char* m_buffer;                     // Global buffer where data is located
};

// This class manages all work with output files
class OutputManager
{
public:
    OutputManager(const std::string& name);
    virtual ~OutputManager();

    void WriteInto(char* buffer, int sizeBuffer, int sizeBlock); // When data is ready for write, it manages writing and hashing
    void printHashIntoFile(char* buffer, int sizeBuffer, int sizeBlock, boost::atomic_int& dataReady); 
private:
    std::ofstream file;
};

void WriteHash(const std::string& name, int sizeBuffer, int sizeBlock);
void ReadBuffer(int sizeBlock, const std::string& name, int sizeBuffer);
