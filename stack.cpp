#include "stack.h"
#include "scheme_std_lib.cpp"
#include <iostream>


TStack::TStack(const std::string fileName)
{
    fin.open(fileName);
    if (!fin.is_open()) {
        return;
    }
    DoCode();
    PrintResult();
}

TStack::~TStack()
{
    fin.close();
}

void TStack::DoCode()
{
    char c;
    std::string line;
    while (!fin.eof()) {
        c = GetChar();
        if (c == '\n') {
            ParseLine(line);
            line = "";
        } else {
            line += c;
        }
    }
}

char TStack::GetChar()
{
    char c;
    fin.get(c);
    return c;
}

void TStack::ParseLine(std::string line)
{
    std::string command;
    size_t i = 0;
    while(line.at(i) != ' ') {
        command += line.at(i);
        i++;
    }
    line.erase(0, i+1);
    if (command == "ADD") {
       stack.push_back(ParseAdd(line));
    } else if (command == "DOFUNC") {
        ParseDoFunc(line);
    }

}

void TStack::ParseDoFunc(std::string func) {
    if (func == "+") {
        stack.push_back(new NumberDoubleType(plus(stack)));
        stack.erase(stack.begin(),stack.end()-1);
    } else if (func == "append") {
        stack.push_back(new ListType(append(stack)));
        stack.erase(stack.begin(),stack.end()-1);
    } else if (func == "cons") {
        stack.push_back(new ListType(cons(stack)));
        stack.erase(stack.begin(),stack.end()-1);
    } else if (func == "list") {
        stack.push_back(new ListType(list(stack)));
        stack.erase(stack.begin(),stack.end()-1);
    }
}

ExprType* TStack::ParseAdd(std::string addLine) {
    EType typeNumber = (EType)(addLine[0]-48);
    addLine.erase(0, 2);
    switch (typeNumber) {
    case T_Ident://
        return (new IdentType(addLine));
    case T_Bool: {
        if (addLine == "#t")  {
            return (new BoolType(true));
        } else {
            return (new BoolType(false));
        }
    }
    case T_String : {
        std::string val = addLine;
        val.erase(0,1);
        val.erase(val.size()-1, val.size());
        return (new StringType(val));
    }
    case T_Symbol : {
        return (new SymbolType(addLine));
    }
    case T_Char : {
        return (new CharType(addLine[0]));
    }
    case T_Int : {
        int val  = std::stoi(addLine);
        return (new NumberIntType(val));
    }
    case T_Double : {
        double val  =  std::stod(addLine);
        return (new NumberDoubleType(val));
    }
    default:
        break;//error
    }

}

void TStack::PrintResult()
{
    ExprType* result = stack[stack.size()-1];
    switch(result->Type) {
    case T_Bool:{
        std::cout << ((BoolType*)result)->value << std::endl;
        break;
    }
    case T_String:{
        std::cout << ((StringType*)result)->value << std::endl;
        break;
    }
    case T_Int:{
        std::cout << ((NumberIntType*)result)->value << std::endl;
        break;
    }
    case T_Double:{
        std::cout << ((NumberDoubleType*)result)->value << std::endl;
        break;
    }
    case T_Char:{
        std::cout << ((CharType*)result)->value << std::endl;
        break;
    }
    case T_Symbol:{
        std::cout << ((SymbolType*)result)->value << std::endl;
        break;
    }
    case T_List:{
        ((ListType*)result)->value->GetListData();
        break;
    }
    }
}
