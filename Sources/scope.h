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
        Class* type_;
        Instance* value_;

        friend class LocalScope;
    public:
        LocalVariable(Class* type, std::string name):
                name_(name),
                type_(type),
                index_(-1),
                owner_(nullptr),
                value_(nullptr){}

        std::string GetName() const{
            return name_;
        }

        void SetOwner(LocalScope* scope){
            owner_ = scope;
        }

        void SetConstantValue(Instance* value){
            value_ = value;
        }

        LocalScope* GetOwner() const{
            return owner_;
        }

        Class* GetType() const{
            return type_;
        }

        Instance* GetConstantValue() const{
            return value_;
        }

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
        LocalScope():
                parent_(nullptr),
                variables_(10),
                sibling_(nullptr),
                child_(nullptr){}

        LocalScope* GetParent() const{
            return parent_;
        }

        bool AddLocal(LocalVariable* local);
        bool HasLocal(std::string name);
        bool LocalLookup(std::string name, LocalVariable** result);
        bool Lookup(std::string name, LocalVariable** result);
    };
}

#endif //ESCHELLE_SCOPE_H
