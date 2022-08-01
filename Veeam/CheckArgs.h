#pragma once
#include <exception>
#include <iostream>
#include <string>
#include <tuple>
#include "Veeam.h"
#include "Consts.h"

struct WrongBlock: public std::exception
{
    const char* what() const throw()
    {
        return "Invalid size of block. It should be between 512 and 10240 Kb.";
    }
};

struct WrongArgs: public std::exception
{
    const char* what() const throw()
    {
        return "Invalid number of args";
    }
};

std::tuple<std::string, std::string, int> CheckCMD(int argc, char *argv[])
{
    if (argc < 3)
    {
        throw WrongArgs();
    }
    int sizeBlock = MB;
    std::string fileInput = argv[1];
    std::string fileOutput = argv[2];

    if (argc == 4)
    {
        sizeBlock = std::stoi(argv[3]);
        // Size of block must be between 512 Kb and 10 Mb
        if (sizeBlock <= 512 || sizeBlock >= 10 * 1024)
            throw WrongBlock();
    }
    return std::make_tuple(fileInput, fileOutput, sizeBlock);
}