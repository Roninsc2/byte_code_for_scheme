#include <iostream>
#include "type.h"

TPairType* GetPairType(ExprType* expr) {
    switch (expr->Type) {
    case T_Bool: {
        return (new TPairTypeBool(((BoolType*)expr)->value));
    }
    case T_String : {
        return (new TPairTypeString(((StringType*)expr)->value));
    }
    case T_Symbol : {
        return (new TPairTypeSymbol(((SymbolType*)expr)->value));
    }
    case T_Char : {
        return (new TPairTypeChar(((CharType*)expr)->value));
    }
    case T_Int : {
        return (new TPairTypeInt(((NumberIntType*)expr)->value));
    }
    case T_Double : {
        return (new TPairTypeDouble(((NumberDoubleType*)expr)->value));
    }
    case T_List : {
        return (new TPairTypeList(((ListType*)expr)->value.get()));
    }
    case T_Ident : {
        return GetPairType(((IdentType*)expr)->value);
    }
    default: {
        break;
        //error
    }
    }
}



VList* list(std::vector< ExprType* > exprs) {
    if (!exprs.size()) {
        return (new VList(TPairTypePtr(nullptr)));
    } else {
        VList* list = new VList(TPairTypePtr(GetPairType(exprs[0])));
        for (size_t i = 1; i < exprs.size(); i++) {
            list->InsertAfter(new VList(TPairTypePtr(GetPairType(exprs[i]))));
        }
        return list;
    }
}

VList* cons(std::vector <ExprType*> expr) {
    TPairTypePtr first(GetPairType(expr[0]));
    TPairTypePtr second(GetPairType(expr[1]));
    VList* list = new VList(first);
    list->ConvetToPair(second.get());
    return list;
}

VList* append(std::vector<ExprType*> expr) {
    if (!expr.size()) {
        return (new VList(TPairTypePtr(nullptr)));
    }
    TPairType* list = GetPairType(expr[0]);
    if (list->GetType() != PT_List) {
        //error
    }
    for (size_t i = 1; i < expr.size()-1; i++) {
        TPairType* addList = GetPairType(expr[i]);
        if (addList->GetType() != PT_List) {
            //error
        } else {
            ((TPairTypeList*)list)->GetValue()->InsertAfter(((TPairTypeList*)addList)->GetValue());
        }
    }
    TPairType* last = GetPairType(expr[expr.size()-1]);
    if (last->GetType() == PT_List) {
       ((TPairTypeList*)list)->GetValue()->InsertAfter(((TPairTypeList*)last)->GetValue());
    } else {
        ((TPairTypeList*)list)->GetValue()->ConvetToPair(last);
    }
}



double plus(std::vector<ExprType*> expr) {
    double result = 0;
    if (!expr.size()) {
        //error
    } else {
        for (size_t i = 0; i < expr.size(); i++) {
            if (expr[i]->Type == T_Int) {
                result += ((NumberIntType*)expr[i])->value;
            } else if (expr[i]->Type == T_Double) {
                result += ((NumberDoubleType*)expr[i])->value;
            } else {
                //error
            }
        }
    }
    std::cout << result << "\n";
    return result;
}



