#include "object.h"

namespace Eschelle{
    Field* Class::DefineField(std::string name, Class *type){
        Field* f = new Field(name, this, type, kNone);
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

    Class* Class::OBJECT = new Class("Object", kNone, nullptr);
    Class* Class::STRING = new Class("String", kFinal, Class::OBJECT);
    Class* Class::BOOLEAN = new Class("Bool", kFinal, Class::OBJECT);
    Class* Class::INTEGER = new Class("Int", kFinal, Class::OBJECT);
    Class* Class::DOUBLE = new Class("Double", kFinal, Class::OBJECT);

    Instance* Instance::New(Class *type){
        word offset = sizeof(Instance);

        for(int i = 0; i < type->fields_.Length(); i++){
            Field* f = type->fields_[i];
            if(!f->IsStatic()){
                f->SetOffset(offset);
                offset += kWordSize;
            }
        }

        std::cout << "Allocating " << offset << " bytes for " << type->GetName() << std::endl;

        void* data = malloc(offset);
        Instance* instance = reinterpret_cast<Instance*>(data);
        instance->type_ = type;
        memset(reinterpret_cast<char*>(data) + sizeof(Instance), 0, offset - sizeof(Instance));
        return instance;
    }

    Boolean* Boolean::TRUE = new Boolean(true);
    Boolean* Boolean::FALSE = new Boolean(false);
}