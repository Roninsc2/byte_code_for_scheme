#pragma once
#include <fstream>
#include <string>
#include <cctype>
#include <vector>
#include "type.h"

class TStack {
public:
    TStack(const std::string fileName);
    ~TStack();
private:
    void DoCode();
    char GetChar();
    void ParseLine(std::string line);
    void ParseDoFunc(std::string func);
    ExprType* ParseAdd(std::string addLine);
    void PrintResult();
private:
    std::ifstream fin;
    std::vector< ExprType* > stack;
};
