#ifndef ESCHELLE_ESCH_VM_H
#define ESCHELLE_ESCH_VM_H

#include <string>
#include "common.h"
#include "array.h"
#include "object.h"

namespace Eschelle{
#define FOR_EACH_VM_CLASS(V) \
    V(Math) \
    V(Vector)

    class EschelleVM{
    private:
        std::string base_path_;
        Array<Class*> loaded_classes_;

        EschelleVM(std::string base_path):
                base_path_(base_path),
                loaded_classes_(10){}

        void LoadFile(const char* file);

        static EschelleVM* instance;
    public:
        static void Init(std::string base_path);
        static void GetLoadedClasses(Array<Class*>* classes);
        static Class* GetLoadedClass(std::string name);
    };
}

#endif //ESCHELLE_ESCH_VM_H