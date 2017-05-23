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
    Class* Test = code->FindClass("Test");

    std::cout << Test->GetName() << std::endl;
    std::cout << "Private?: " << Test->IsPrivate() << std::endl;
    std::cout << "Fields: " << Test->GetFieldCount() << std::endl;
    for(int i = 0; i < Test->GetFieldCount(); i++){
        std::cout << "\t#" << i << ": " << Test->GetFieldAt(i)->GetName() << " - " << Test->GetFieldAt(i)->GetFieldType()->GetName() << std::endl;
    }

    Class* Test2 = code->FindClass("Test2");

    std::cout << Test2->GetName() << std::endl;
    std::cout << "Private?: " << Test2->IsPrivate() << std::endl;
    std::cout << "Fields: " << Test2->GetFieldCount() << std::endl;
    for(int i = 0; i < Test2->GetFieldCount(); i++){
        std::cout << "\t#" << i << ": " << Test2->GetFieldAt(i)->GetName() << " - " << Test2->GetFieldAt(i)->GetFieldType()->GetName() << std::endl;
    }

    Class* Test3 = code->FindClass("Test3");

    std::cout << Test3->GetName() << std::endl;
    std::cout << "Private?: " << Test3->IsPrivate() << std::endl;
    std::cout << "Fields: " << Test3->GetFieldCount() << std::endl;
    for(int i = 0; i < Test3->GetFieldCount(); i++){
        std::cout << "\t#" << i << ": " << Test3->GetFieldAt(i)->GetName() << " - " << Test3->GetFieldAt(i)->GetFieldType()->GetName() << std::endl;
    }
    std::cout << "Functions: " << Test3->GetFunctionCount() << std::endl;
    for(int i = 0; i < Test3->GetFunctionCount(); i++){
        Function* func = Test3->GetFunctionAt(i);
        std::cout << "\t#" << i << ": " << func->GetName() << " - " << func->GetReturnType()->GetName() << std::endl;
        AstPrinter printer;
        func->GetAst()->Visit(&printer);
    }
    return 0;
}