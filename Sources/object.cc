#include "object.h"
#include "alloc.h"

namespace Eschelle{
    Field* Class::DefineField(std::string name, Class *type){
        Field* f = new Field(name, this, type, kNone);
        fields_.Add(f);
        return f;
    }

    Field* Class::DefineStaticField(std::string name, Class* type){
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
}