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
    std::map<String,antlrcpp::Any*> table;
    struct variableTable* parent;
    variableTable(struct variableTable* prt){
        parent = prt;
        table=std::map<String,antlrcpp::Any*>();
    }
public:
    void* get(String tar);
    void put(String name,antlrcpp::Any* tar);

}VariableTable;

void *VariableTable::get(String tar) {
    void* target = table[tar];
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

void VariableTable::put(String name,antlrcpp::Any* tar){
    table[name]=tar;
}

#endif //ANTPIE_VARIABLETABLE_H
