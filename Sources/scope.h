#ifndef ESCHELLE_SCOPE_H
#define ESCHELLE_SCOPE_H

#include "common.h"
#include "object.h"

namespace Eschelle{
    class LocalScope;

    class LocalVariable{
    private:
        std::string name_;
        LocalScope* owner_;
        int index_;
        Instance* value_;

        friend class LocalScope;
    public:
        LocalVariable(std::string name):
                name_(name),
                index_(-1),
                owner_(nullptr),
                value_(nullptr){}

        bool IsConstant() const{
            return value_ != nullptr;
        }

        bool HasIndex(){
            return index_ != -1;
        }
    };

    class LocalScope{
    private:
        LocalScope* parent_;
        LocalScope* child_;
        LocalScope* sibling_;
        Array<LocalVariable*> variables_;
    public:
        LocalScope(LocalScope* parent):
                parent_(parent),
                variables_(10),
                sibling_(nullptr),
                child_(nullptr){
            if(parent != nullptr){
                sibling_ = parent->child_;
                parent->child_ = this;
            }
        }

        bool LocalLookup(std::string name, LocalVariable** result);
        bool Lookup(std::string name, LocalVariable** result);
    };
}

#endif //ESCHELLE_SCOPE_H
