#pragma once
#include <string>
#include <vector>
#include <memory>
#include <vlist.h>
#include <map>


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
    size_t level;
};

class NumberIntType : public ExprType {

public:
    NumberIntType(int val, size_t lvl) : value(val) {
        Type = T_Int;
        level = lvl;
    }

public:
    int value;
};

class NumberDoubleType : public ExprType {

public:
    NumberDoubleType(double val, size_t lvl) : value(val) {
        Type = T_Double;
        level = lvl;
    }

public:
    double value;
};


class IdentType : public ExprType {

public:
    IdentType(std::string val, size_t lvl) : name(val) {
        Type = T_Ident;
        level = lvl;
    }
    ~IdentType() {
        delete value;
    }

public:
    std::string name;
    ExprType* value = nullptr;
};

class StringType : public ExprType {

public:
    StringType(std::string val, size_t lvl) : value(val) {
        Type = T_String;
        level = lvl;
    }

public:
    std::string value;
};

class CharType : public ExprType {

public:
    CharType(char val, size_t lvl) : value(val) {
        Type = T_Char;
        level = lvl;
    }

public:
    char value;
};

class BoolType : public ExprType {

public:
    BoolType(bool val, size_t lvl) : value(val) {
        Type = T_Bool;
        level = lvl;
    }

public:
    bool value;
};

class SymbolType : public ExprType {

public:
    SymbolType(std::string val, size_t lvl) : value(val) {
        Type = T_Symbol;
        level = lvl;
    }

public:
    std::string value;
};

class ListType : public ExprType {

public:
    ListType(VList* val, size_t lvl) : value(val) {
        Type = T_List;
        level = lvl;
    }

public:
    std::shared_ptr<VList> value;
};


class PrototypeType {
public:
  PrototypeType(const std::string &name, const std::map<std::string, IdentType*> &args)
    : Name(name), Args(args)
  {
  }
public:
  std::string Name;
  std::map<std::string, IdentType*> Args;
};


class FunctionType {
public:
  FunctionType(PrototypeType* proto,  std::vector< std::string > body)
    : Proto(proto), Body(body)
  {
  }
public:
  std::shared_ptr< PrototypeType >Proto;
  std::vector< std::string >Body;
};
