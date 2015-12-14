#pragma once
#include <fstream>
#include <string>
#include <cctype>
#include <vector>
#include <map>
#include "type.h"
#include "saveload.h"

class TStack {
public:
    TStack(const std::string fileName);
    ~TStack();
private:
    void DoCode();
    bool Allocator();
    ExprType* ParseCmdPush();
    ExprType* ParseCmdPushIdent();
    ExprType* ParseCmdCall();
    ExprType* ParseCmdTailCall();
    std::pair<std::string, FunctionType*>  ParseCmdDefine();
    ExprType* ParseCmdIfElse();
    void PrintResult();
private:
    std::ifstream fin;
    std::string Buffer;
    std::vector< ExprType* > stack;
    std::vector< ExprType* > allocator;
    std::map<std::string, IdentType* > defineVar;
    std::map<std::string, FunctionType* > defineFunc;
    std::map<std::string, ExprType*(*)(std::vector< ExprType* >) > stdFuncMap;
    enum Command {
        CMD_AllOC = '0',
        CMD_PUSH = '1',
        CMD_PUSHIDENT = '2',
        CMD_IFELSE = '3',
        CMD_CALL = '4',
        CMD_TAILCALL = '5',
        CMD_DEFSTART = '6',
        CMD_ENDCALL = '7',
        CMD_ENDDEF = '8',
    };

    enum ValueType {
        VT_INT = '0',
        VT_DOUBLE = '1',
        VT_STRING = '2',
        VT_SYMBOL = '3',
        VT_CHAR = '4',
        VT_BOOL = '5'
    };
};
