#include <iostream>
#include "object.h"

using namespace Eschelle;

int
main(int argc, char** argv){
    Class* main = new Class("Main", kFinal);

    Field* pi = main->DefineField("pi", Class::DOUBLE);
    Field* tau = main->DefineField("tau", Class::DOUBLE);
    Field* test = main->DefineField("test", Class::BOOLEAN);

    Instance* mainInstance = Instance::New(main);

    std::cout << "Allocation Size: " << main->GetAllocationSize() << std::endl;
    std::cout << "\tInstance: " << sizeof(Instance) << "\tFields: (" << main->GetFieldCount() << " * " << kWordSize << ")" << std::endl;
    std::cout << "\tTotal: " << (sizeof(Instance) + main->GetAllocationSize()) << std::endl;
}