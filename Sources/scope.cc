#include "scope.h"

namespace Eschelle{
    bool LocalScope::Lookup(std::string name, LocalVariable **result){
        LocalScope* scope = this;
        while(scope != nullptr){
            if(LocalLookup(name, result)){
                return true;
            }
            scope = scope->parent_;
        }
        *result = nullptr;
        return false;
    }

    bool LocalScope::LocalLookup(std::string name, LocalVariable **result){
        for(int i = 0; i < variables_.Length(); i++){
            if(variables_[i]->name_ == name){
                *result = variables_[i];
                return true;
            }
        }
        *result = nullptr;
        return false;
    }
}