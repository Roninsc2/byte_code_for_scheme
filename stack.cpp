#include "stack.h"
#include "scheme_std_lib.cpp"
#include <iostream>


TStack::TStack(const std::string fileName)
{
    fin.open(fileName, std::ios::binary|std::ios::in);
    if (!fin.is_open()) {
        return;
    }
    fin.seekg(0, fin.end);
    size_t len = fin.tellg();
    fin.seekg(0, fin.beg);
    Buffer.resize(len);
    fin.read((char*)(&Buffer[0]), len);
    while (Allocator()) {
    }
    stdFuncMap.insert(std::make_pair("+", &plus));
    stdFuncMap.insert(std::make_pair("-", &minus));
    stdFuncMap.insert(std::make_pair("/", &division));
    stdFuncMap.insert(std::make_pair("*", &mult));
    stdFuncMap.insert(std::make_pair("cons", &cons));
    stdFuncMap.insert(std::make_pair("append", &append));
    stdFuncMap.insert(std::make_pair("list", &list));
    stdFuncMap.insert(std::make_pair("=", &equally));
    stdFuncMap.insert(std::make_pair("define", &defineFun));
    DoCode();
    PrintResult();
}

TStack::~TStack()
{
    fin.close();
}

void TStack::DoCode()
{
    while (Buffer.length() > 0) {
        Command cmd = (Command)Buffer.at(0);
        switch (cmd) {
        case CMD_PUSH: {
            stack.push_back(ParseCmdPush());
            break;
        }
        case CMD_PUSHIDENT: {
            stack.push_back(ParseCmdPushIdent());
            break;
        }
        case CMD_CALL: {
            stack.push_back(ParseCmdCall());
            break;
        }
        case CMD_DEFSTART: {
            defineFunc.insert(ParseCmdDefine());
            break;
        }
        case CMD_IFELSE: {
            stack.push_back(ParseCmdIfElse());
            break;
        }
        case CMD_TAILCALL: {
            stack.push_back(ParseCmdTailCall());
            break;
        }
        default:
            break;
        }
    }
}

bool TStack::Allocator()
{
    size_t size = *(unsigned short*)(Buffer.data());
    Command allocCMD = (Command)Buffer.at(0);
    if (allocCMD != CMD_AllOC) {
        return false;
    }
    ValueType allocType = (ValueType)Buffer.at(1);
    imemstream in(Buffer.data() + 2, size - 1);
    switch (allocType) {
    case VT_INT: {
        int val;
        LoadMany(in, val);
        allocator.push_back(new NumberIntType(val));
        Buffer.erase(0, 2+sizeof(int));
        break;
    }
    case VT_DOUBLE: {
        double val;
        LoadMany(in, val);
        allocator.push_back(new NumberDoubleType(val));
        Buffer.erase(0, 2+sizeof(double));
        break;
    }
    case VT_STRING: {
        std::string val;
        LoadMany(in, val);
        allocator.push_back(new StringType(val));
        Buffer.erase(0, 4+val.length());
        break;
    }
    case VT_SYMBOL: {
        std::string val;
        LoadMany(in, val);
        allocator.push_back(new SymbolType(val));
        Buffer.erase(0, 4 + val.length());
        break;
    }
    case VT_CHAR: {
        char val;
        LoadMany(in, val);
        allocator.push_back(new CharType(val));
        Buffer.erase(0, 2+sizeof(char));
        break;
    }
    case VT_BOOL: {
        bool val;
        LoadMany(in, val);
        allocator.push_back(new BoolType(val));
        Buffer.erase(0, 2+sizeof(bool));
        break;
    }
    default:
        break;
    }
    return true;
}

ExprType *TStack::ParseCmdPush()
{
    size_t size = *(unsigned short*)(Buffer.data());
    imemstream in(Buffer.data() + 1, size - 1);
    size_t val;
    LoadMany(in, val);
    Buffer.erase(0, 1+sizeof(size_t));
    return allocator.at(val);
}

ExprType *TStack::ParseCmdPushIdent()
{
    size_t size = *(unsigned short*)(Buffer.data());
    imemstream in(Buffer.data() + 1, size - 1);
    std::string val;
    LoadMany(in, val);
    Buffer.erase(0, 3+val.length());
    return defineVar.at(val)->value;
}

ExprType* TStack::ParseCmdCall()
{
    size_t size = *(unsigned short*)(Buffer.data());
    imemstream in(Buffer.data() + 1, size - 1);
    std::string name;
    LoadMany(in, name);
    Buffer.erase(0, 3 + name.length());
    std::vector<ExprType*> exprs;
    while (Buffer.length() > 0) {
        Command cmd = (Command)Buffer.at(0);
        if (cmd == CMD_ENDCALL) {
            Buffer.erase(0, 1);
            break;
        }
        switch (cmd) {
        case CMD_PUSH: {
            exprs.push_back(ParseCmdPush());
            break;
        }
        case CMD_PUSHIDENT: {
            exprs.push_back(ParseCmdPushIdent());
            break;
        }
        case CMD_CALL: {
            exprs.push_back(ParseCmdCall());
            break;
        }
        case CMD_IFELSE: {
            exprs.push_back(ParseCmdIfElse());
            break;
        }
        case CMD_TAILCALL: {
            exprs.push_back(ParseCmdTailCall());
            break;
        }
        default:
            break;
        }
    }
    if (name == "define") {
        IdentType* ident = (IdentType*)stdFuncMap.at(name)(exprs);
        defineVar.insert(std::make_pair(ident->name, ident));
    }
    if (stdFuncMap.count(name)) {
        return (stdFuncMap.at(name)(exprs));
    } else if (defineFunc.count(name)) {
        if (exprs.size() == defineFunc.at(name)->Proto->Args.size()) {
            size_t j = 0;
            ExprType* result;
            std::map<std::string, IdentType* > defineVarBuffer = defineVar;
            std::map<std::string, FunctionType* > defineFuncBuffer = defineFunc;
            for (auto it = defineFunc.at(name)->Proto->Args.begin(); it != defineFunc.at(name)->Proto->Args.end(); it++) {
                it->second->value = exprs.at(j);
                if (defineVar.count(it->first)) {
                    defineVar.erase(it->first);
                }
                defineVar.insert(std::make_pair(it->first, it->second));
                j++;
            }
            std::string BufferTemp = Buffer;
            Buffer = defineFunc.at(name)->Body;
            while (Buffer.length() > 0) {
                Command cmd = (Command)Buffer.at(0);
                switch (cmd) {
                case CMD_PUSH: {
                    result = ParseCmdPush();
                    break;
                }
                case CMD_PUSHIDENT: {
                    result = ParseCmdPushIdent();
                    break;
                }
                case CMD_CALL: {
                    result = ParseCmdCall();
                    break;
                }
                case CMD_DEFSTART: {
                    std::pair<std::string, FunctionType*> currentFunc = ParseCmdDefine();
                    if (defineFunc.count(currentFunc.first)) {
                        defineFunc.erase(currentFunc.first);
                    }
                    defineFunc.insert(currentFunc);
                    break;
                }
                case CMD_IFELSE: {
                    result = ParseCmdIfElse();
                    break;
                }
                case CMD_TAILCALL: {
                    result = ParseCmdTailCall();
                    break;
                }
                default:
                    break;
                }
            }
            defineVar = defineVarBuffer;
            defineFunc = defineFuncBuffer;
            Buffer = BufferTemp;
            return result;
        }
    }
}

ExprType *TStack::ParseCmdTailCall()
{

}

std::pair<std::string, FunctionType*> TStack::ParseCmdDefine()
{
    size_t size = *(unsigned short*)(Buffer.data());
    imemstream in(Buffer.data() + 1, size - 1);
    std::string funcName;
    size_t sizeArgs;
    LoadMany(in, funcName, sizeArgs);
    std::map<std::string, IdentType*> args;
    size_t forDelete = 3 + funcName.length() + sizeof(size_t);
    for (size_t i = 0; i < sizeArgs; i++) {
        std::string identName;
        LoadMany(in, identName);
        args.insert(std::make_pair(identName, new IdentType(identName)));
        forDelete += 2 + identName.length();
    }
    Buffer.erase(0, forDelete);
    PrototypeType* proto = new PrototypeType(funcName, args);
    std::string Body;
    while (Buffer.length() > 0) {
        Command cmd = (Command)Buffer.at(0);
        if (cmd == CMD_ENDDEF) {
            Buffer.erase(0, 1);
            break;
        }
        Body += Buffer.at(0);
        Buffer.erase(0,1);
    }
    return (std::make_pair(funcName ,new FunctionType(proto, Body)));
}

ExprType *TStack::ParseCmdIfElse()
{

}

void TStack::PrintResult()
{
    ExprType* expr = stack.at(stack.size()-1);
    switch (expr->Type) {
    case T_Int: {
        std::cout << ((NumberIntType*)expr)->value << std::endl;
        break;
    }
    case T_Double: {
        std::cout << ((NumberDoubleType*)expr)->value << std::endl;
        break;
    }
    case T_Symbol: {
        std::cout << ((SymbolType*)expr)->value << std::endl;
        break;
    }
    case T_String: {
        std::cout << ((StringType*)expr)->value << std::endl;
        break;
    }
    case T_Char: {
        std::cout << ((CharType*)expr)->value << std::endl;
        break;
    }
    case T_Bool: {
        std::cout << ((BoolType*)expr)->value << std::endl;
        break;
    }
    case T_List: {
        ((ListType*)expr)->value->GetListData();
        break;
    }
    default:
        break;
    }
}
