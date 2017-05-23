#include "object.h"
#include "alloc.h"
#include "ast.h"

namespace Eschelle{
    Field* Class::DefineField(std::string name, Class *type, bool priv){
        Field* f = new Field(name, this, type, kNone);
        fields_.Add(f);
        return f;
    }

    Field* Class::DefineStaticField(std::string name, Class* type, bool priv){
        Field* f = new Field(name, this, type, kStatic);
        fields_.Add(f);
        return f;
    }

    Field* Class::GetField(std::string name){
        for(int i = 0; i < fields_.Length(); i++){
            Field* f = fields_[i];
            if(f->GetName() == name){
                return f;
            }
        }
        return nullptr;
    }

    Function* Class::DefineFunction(std::string name, Class *ret_type, bool priv){
        int mods = (priv ? kPrivate : kNone);
        Function* func = new Function(name, mods, this, ret_type);
        functions_.Add(func);
        return func;
    }

    Function* Class::DefineStaticFunction(std::string name, Class *ret_type, bool priv){
        int mods = kStatic;
        if(priv) mods |= kPrivate;
        Function* func = new Function(name, mods, this, ret_type);
        functions_.Add(func);
        return func;
    }

    Function* Class::GetFunction(std::string name){
        for(int i = 0; i < functions_.Length(); i++){
            if(functions_[i]->GetName() == name){
                return functions_[i];
            }
        }
        return nullptr;
    }

    word Class::GetAllocationSize(){
        word offset = sizeof(Instance);
        for(int i = 0; i < fields_.Length(); i++){
            Field* f = fields_[i];
            if(!f->IsStatic()){
                offset += kWordSize;
            }
        }
        return offset;
    }

    Class* Class::OBJECT = new Class("Object", kNone, nullptr);
    Class* Class::STRING = new Class("String", kFinal, Class::OBJECT);
    Class* Class::BOOLEAN = new Class("Bool", kFinal, Class::OBJECT);
    Class* Class::INTEGER = new Class("Int", kFinal, Class::OBJECT);
    Class* Class::DOUBLE = new Class("Double", kFinal, Class::OBJECT);
    Class* Class::VOID = new Class("Void", kFinal, Class::VOID);

    Instance* Instance::New(Class *type){
        void* ptr = Allocator::Alloc(type->GetAllocationSize());

        Instance* fake = new Instance(type);
        Instance* instance = reinterpret_cast<Instance*>(ptr);
        memcpy(instance, fake, sizeof(Instance));
        delete fake;

        return instance;
    }

    Bool* Bool::TRUE = new Bool(true);
    Bool* Bool::FALSE = new Bool(false);

    Function::Function(std::string name, int mods, Class *owner, Class *ret_type):
            name_(name),
            mods_(mods),
            code_(),
            ast_(new SequenceNode()),
            result_type_(ret_type),
            owner_(owner){}

    void Function::AddAst(AstNode* node){
        ast_->Add(node);
    }
}