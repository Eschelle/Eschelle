#ifndef ESCHELLE_CONTROL_FLOW_ALLOC_H
#define ESCHELLE_CONTROL_FLOW_ALLOC_H

#include "common.h"
#include "flow_graph.h"
#include "irep.h"

namespace Eschelle{
    struct LiveRange{
        word start;
        word end;
        word vreg;
    };

    class InterferenceGraph{
    private:
        FlowGraph* graph_;
        Array<LiveRange*> all_ranges_;
        Array<LiveRange*> active_ranges_;
        Array<word> registers_pool_;
        Array<word> registers_map_;
        Array<Instruction*> instructions_;
        Array<BlockEntryInstr*> postorder_;
    public:
        InterferenceGraph(FlowGraph* graph):
                graph_(graph),
                all_ranges_(graph->GetCurrentSSATempIndex()),
                active_ranges_(graph->GetCurrentSSATempIndex()),
                registers_pool_(10),
                registers_map_(10),
                instructions_(10),
                postorder_(10){}
    };
}

#endif //ESCHELLE_CONTROL_FLOW_ALLOC_H
