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
    } else if (command == "DEFINE") {
        ParseDefine(line);
    }

}

void TStack::ParseDoFunc(std::string func) {
    size_t lvl = func[0]-48;
    std::vector <ExprType*> expr;
    std::vector <ExprType* > newStack;
    for (size_t i = 0; i < stack.size(); i++) {
        if (stack[i]->level == lvl) {
            expr.push_back(stack[i]);
        }
    }
    func.erase(0,2);
    for (size_t i = 0; i < stack.size(); i++) {
        if (stack[i]->level != lvl) {
            newStack.push_back(stack[i]);
        }
    }
    size_t offset;
    if (lvl == 0) {
        offset = 0;
    } else {
        offset = 1;
    }
    if (func == "+") {
        newStack.push_back(new NumberDoubleType(plus(expr), lvl-offset));
    } else if (func == "append") {
        newStack.push_back(new ListType(append(expr), lvl-offset));
    } else if (func == "cons") {
        newStack.push_back(new ListType(cons(expr), lvl-offset));
    } else if (func == "list") {
        newStack.push_back(new ListType(list(expr), lvl-offset));
    } else if (func == "-") {
        newStack.push_back(new NumberDoubleType(minus(expr), lvl-offset));
    } else if (func == "*") {
        newStack.push_back(new NumberDoubleType(mult(expr), lvl-offset));
    } else if (func == "/") {
        newStack.push_back(new NumberDoubleType(division(expr), lvl-offset));
    } else if (func == "define") {
        defineVar.insert(defineFun(expr));
    } else if (defineFunc.count(func)) {
        if (defineFunc.at(func)->Proto->Args.size() == expr.size()) {
            size_t j = 0;
            for (std::map<std::string, IdentType*>::iterator i = defineFunc.at(func)->Proto->Args.begin(); i != defineFunc.at(func)->Proto->Args.end(); i++) {
                i->second->value = expr[j];
                j++;
            }
        }
        std::map<std::string, IdentType* > defineVarTemp = defineFunc.at(func)->Proto->Args;
        defineVar.swap(defineVarTemp);
        for (size_t i = 0; i < defineFunc.at(func)->Body.size(); i++) {
            ParseLine(defineFunc.at(func)->Body[i]);
        }
        defineVar = defineVarTemp;
    } else {
        return;
    }
    stack = newStack;
}

void TStack::ParseDefine(std::string line)
{
    std::string name;
    size_t i = 0;
    while (line[i] != ' ') {
        name += line[i];
        i++;
    }
    i++;
    std::map<std::string, IdentType*> args;
    std::string argName;
    while (true) {
        if (line[i] == '\0') {
            break;
        }
        if (line[i] == ' ') {
            args.insert(std::pair< std::string, IdentType*>(argName, new IdentType(argName, 0)));
            argName = "";
        } else {
            argName += line[i];
        }
        i++;
    }
    char c;
    line = "";
    std::vector<std::string> body;
    while (true) {
        c = GetChar();
        if (fin.eof() || line == "ENDDEFINE") {
            GetChar();
            //error
            break;
        }
        if (c == '\n') {
            body.push_back(line);
            line = "";
        } else {
            line += c;
        }
    }
    defineFunc.insert(std::pair<std::string, FunctionType*>(name, new FunctionType(new PrototypeType(name, args), body)));

}

ExprType* TStack::ParseAdd(std::string addLine) {
    size_t level = (size_t)(addLine[0]-48);
    EType typeNumber = (EType)(addLine[2]-48);
    addLine.erase(0, 4);
    switch (typeNumber) {
    case T_Ident://
        if (defineVar.count(addLine)) {
            return defineVar.at(addLine);
        }
        return (new IdentType(addLine, level));
    case T_Bool: {
        if (addLine == "#t")  {
            return (new BoolType(true, level));
        } else {
            return (new BoolType(false, level));
        }
    }
    case T_String : {
        std::string val = addLine;
        val.erase(0,1);
        val.erase(val.size()-1, val.size());
        return (new StringType(val, level));
    }
    case T_Symbol : {
        return (new SymbolType(addLine, level));
    }
    case T_Char : {
        return (new CharType(addLine[0], level));
    }
    case T_Int : {
        int val  = std::stoi(addLine);
        return (new NumberIntType(val, level));
    }
    case T_Double : {
        double val  =  std::stod(addLine);
        return (new NumberDoubleType(val, level));
    }
    default:
        break;//error
    }
}

void TStack::PrintResult()
{
    ExprType* result = stack[0];
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
