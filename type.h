#pragma once
#include <string>
#include <vector>
#include <memory>
#include <vlist.h>


enum EType {
    T_Int,//0
    T_Double,//1
    T_String,//2
    T_Symbol,//3
    T_Char,//4
    T_Bool,//5
    T_List, //6
    T_Ident,//7
};

class ExprType {
public:
    virtual ~ExprType(){}
    EType Type;
};

class NumberIntType : public ExprType {

public:
    NumberIntType(int val) : value(val) {
        Type = T_Int;
    }

public:
    int value;
};

class NumberDoubleType : public ExprType {

public:
    NumberDoubleType(double val) : value(val) {
        Type = T_Double;
    }

public:
    double value;
};


class IdentType : public ExprType {

public:
    IdentType(std::string val) : name(val) {
        Type = T_Ident;
    }

public:
    std::string name;
    ExprType* value;
};

class StringType : public ExprType {

public:
    StringType(std::string val) : value(val) {
        Type = T_String;
    }

public:
    std::string value;
};

class CharType : public ExprType {

public:
    CharType(char val) : value(val) {
        Type = T_Char;
    }

public:
    char value;
};

class BoolType : public ExprType {

public:
    BoolType(bool val) : value(val) {
        Type = T_Bool;
    }

public:
    bool value;
};

class SymbolType : public ExprType {

public:
    SymbolType(std::string val) : value(val) {
        Type = T_Symbol;
    }

public:
    std::string value;
};

class ListType : public ExprType {

public:
    ListType(VList* val) : value(val) {
        Type = T_List;
    }

public:
    std::shared_ptr<VList> value;
};

