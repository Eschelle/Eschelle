#include "parser.h"
#include "esch_vm.h"

namespace Eschelle{
    EschelleVM* EschelleVM::instance = nullptr;

    void EschelleVM::LoadFile(const char *file){
#if ESCH_DEBUG
        std::cout << "Loading file: " << base_path_ << "/esch/lang/" << file << std::endl;
#endif // ESCH_DEBUG

        Parser parser;
        CodeUnit* code = parser.Parse(base_path_ + "/esch/lang/" + std::string(file) + ".esch");
        for(int i = 0; i < code->GetClassesSize(); i++) loaded_classes_.Add(code->GetClassAt(i));
    }

    void EschelleVM::Init(std::string base_path){
        instance = new EschelleVM(base_path);

#define LOAD_CLASS(Name) instance->LoadFile(#Name);
FOR_EACH_VM_CLASS(LOAD_CLASS)
#undef LOAD_CLASS
    }

    void EschelleVM::GetLoadedClasses(Array<Class*>* classes){
        for(int i = 0; i < instance->loaded_classes_.Length(); i++){
            classes->Add(instance->loaded_classes_[i]);
        }
    }

    Class* EschelleVM::GetLoadedClass(std::string name){
        for(int i = 0; i < instance->loaded_classes_.Length(); i++){
            if(instance->loaded_classes_[i]->GetName() == name){
                return instance->loaded_classes_[i];
            }
        }
        return nullptr;
    }
}