#include "object.h"
#include "alloc.h"
#include "ast.h"
#include "ast_printer.h"

namespace Eschelle{
    Field* Class::CreateField(std::string name, Class *type, bool priv){
        return IsFinal() ?
               DefineStaticField(name, type, priv) :
               DefineField(name, type, priv);
    }

    Field* Class::DefineField(std::string name, Class *type, bool priv){
        Field* f = new Field(name, this, type, (priv ? kPrivate : kNone));
        fields_.Add(f);
        return f;
    }

    Field* Class::DefineStaticField(std::string name, Class* type, bool priv){
        int mods = kStatic;
        if(priv) mods |= kPrivate;
        Field* f = new Field(name, this, type, mods);
        fields_.Add(f);
        return f;
    }

    AstNode* Field::CreateStore(AstNode *value){
        if(IsStatic()){
            return new StoreStaticFieldNode(this, value);
        } else{
            return new StoreInstanceFieldNode(this, value);
        }
    }

    AstNode* Field::CreateLoad(){
        if(IsStatic()){
            return new LoadStaticFieldNode(this);
        } else{
            return new LoadInstanceFieldNode(this);
        }
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
        Function* func = new Function(name, (priv ? kPrivate : kNone), this, ret_type);
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

    void Function::AttachScope(LocalScope *scope){
        ast_->SetScope(scope);
    }

#if defined(ESCH_DEBUG)
    std::ostream& operator<<(std::ostream &stream, const Class& cls){
        stream << cls.GetName() << std::endl;
        stream << "Type: ";
        switch(cls.GetType()){
            case kClassClass: stream << "Class" << std::endl; break;
            case kEnumClass: stream << "Enum" << std::endl; break;
            case kObjectClass: stream << "Object" << std::endl; break;
            case kProtoClass: stream << "Prototype" << std::endl; break;
            default: stream << "Unknown" << std::endl; break;
        }
        stream << "Fields (" << cls.GetFieldCount() << "):" << std::endl;
        for(int i = 0; i < cls.GetFieldCount(); i++){
            Field* f = cls.GetFieldAt(i);
            stream << "\t#" << i << ": " << f->GetName() << " ";
            if(f->IsStatic() || f->IsFinal() || f->IsPrivate()) stream << "-";
            if(f->IsStatic()){
                stream << "S";
            }
            if(f->IsFinal()){
                stream << "F";
            }
            if(f->IsPrivate()){
                stream << "P";
            }
            stream << std::endl;
        }
        stream << std::endl;
        stream << "Functions (" << cls.GetFunctionCount() << "):" << std::endl;
        for(int i = 0; i < cls.GetFunctionCount(); i++){
            Function* func = cls.GetFunctionAt(i);

            std::string name = func->GetName() == cls.GetName() ?
                               "Constructor" :
                               func->GetName();

            stream << "\t#" << i << ": " << name;
            if(!func->IsAbstract()){
                stream << std::endl;
                AstPrinter printer(std::cout);
                func->GetAst()->Visit(&printer);
                stream << std::endl;
            } else{
                stream << " - Abstract" << std::endl;
            }
        }
        return stream;
    }
#endif // ESCH_DEBUG
}