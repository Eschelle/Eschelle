#include "object.h"
#include "alloc.h"
#include "asm.h"
#include "parser.h"
#include "ast_printer.h"

using namespace Eschelle;

int
main(int argc, char** argv){
    /*
    Class* cls = new Class("Test", kNone);
    Field* isTest = cls->DefineField("isTest", Class::BOOLEAN);

    Instance* i = Instance::New(cls);
    i->SetField(isTest, Bool::TRUE);

    Allocator::PrintHeap(kEden);
    */

    /*
    Assembler assm;
    assm.movq(RAX, Immediate(10));
    assm.movq(RBX, Immediate(100));
    assm.addq(RAX, RBX);
    assm.ret();

    MemoryRegion* region;
    assm.Compile(&region);

    typedef int (*Function)(void);

    Function f = reinterpret_cast<Function>(region->Pointer());

    std::cout << f() << std::endl;
     */

    Parser* parser = new Parser();
    CodeUnit* code = parser->Parse(std::string(argv[1]));

    Class* cls1 = code->FindClass("Test");
    std::cout << *cls1 << std::endl;
    Class* cls2 = code->FindClass("Test2");
    std::cout << *cls2 << std::endl;
    Class* cls3 = code->FindClass("Test3");
    std::cout << *cls3 << std::endl;
    return 0;
}