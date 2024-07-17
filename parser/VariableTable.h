//
// Created by yanayn on 7/9/24.
//

#ifndef ANTPIE_VARIABLETABLE_H
#define ANTPIE_VARIABLETABLE_H
#define String std::string
#define any antlrcpp::Any
#include <map>
#include <string>

typedef struct variableTable{
    std::map<String,Value*> table;
    vector<string> FParams;
    struct variableTable* parent;
    variableTable(struct variableTable* prt){
        parent = prt;
        table=std::map<String,Value*>();
    }
public:
    Value* get(String tar);
    void put(String name,Value* tar);

}VariableTable;

Value *VariableTable::get(String tar) {
    Value* target = table[tar];
    if(target== nullptr){
        if(parent!= nullptr){
            return parent->get(tar);
        }else{
            return nullptr;
        }
    }else{
        return target;
    }
}

void VariableTable::put(String name,Value* tar){
    table[name]=tar;
}


#endif //ANTPIE_VARIABLETABLE_H
