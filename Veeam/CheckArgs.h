#pragma once
#include <exception>
#include <iostream>
#include <string>
#include <tuple>
#include "Veeam.h"
#include "consts.h"

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
    std::string fileInput, fileOutput;
    int sizeBlock = MB;
    fileInput = argv[1];
    fileOutput = argv[2];
// pochemy tak
    if (argc == 4)
    {
        sizeBlock = std::stoi(argv[3]);
        if (sizeBlock <= 512 || sizeBlock >= 10 * 1024)
            throw WrongBlock();
    }

    return std::make_tuple(fileInput, fileOutput, sizeBlock);
}