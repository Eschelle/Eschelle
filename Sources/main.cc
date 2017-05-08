#include "object.h"
#include "alloc.h"

using namespace Eschelle;

int
main(int argc, char** argv){
    Class* cls = new Class("Test", kNone);
    Field* isTest = cls->DefineField("isTest", Class::BOOLEAN);

    Instance* i = Instance::New(cls);
    i->SetField(isTest, Bool::TRUE);

    Allocator::PrintHeap(kEden);
}