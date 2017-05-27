#include "object.h"
#include "parser.h"
#include "asm.h"
#include "esch_vm.h"
#include "flow_graph_builder.h"

using namespace Eschelle;

int
main(int argc, char** argv){
    /*
    Class* cls = new Class("Test", kNone);
    Field* isTest = cls->DefineField("isTest", Class::BOOLEAN, false);
    Field* isTest2 = cls->DefineField("isTest2", Class::BOOLEAN, false);

    Instance* i = Instance::New(cls);
    i->SetField(isTest, Bool::TRUE);
    i->SetField(isTest2, Bool::FALSE);

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

    std::cout << getOneHundredAndTen() << std::endl;
    */

    std::string base(argv[1]);
    EschelleVM::Init(base + "/Core");

    Parser* parser = new Parser();
    CodeUnit* code = parser->Parse((base + "/" + std::string(argv[2])));

    std::cout << std::endl;
    std::cout << *(code->FindClass("Modifier")) << std::endl;
    std::cout << *(code->FindClass("Field")) << std::endl;
    std::cout << *(code->FindClass("Main")) << std::endl;

    Function* main = code->FindClass("Main")->GetFunction("main");
    FlowGraphBuilder builder(main);
    FlowGraph* graph = builder.BuildGraph();
    // graph->DiscoverBlocks();
    // graph->ComputeSSA(0);

    Instruction* instr = graph->GetEntry()->SuccessorAt(0);
    while(instr != nullptr){
        std::cout << instr->Name() << std::endl;
        instr = instr->GetNext();
    }

    return 0;
}