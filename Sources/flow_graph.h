#ifndef ESCHELLE_CONTROL_FLOW_H
#define ESCHELLE_CONTROL_FLOW_H

#include "common.h"
#include "object.h"
#include "irep.h"
#include "bit_vector.h"

namespace Eschelle{
    class FlowGraph{
    private:
        Function* function_;
        GraphEntryInstr* graph_entry_;
        word current_ssa_temp_index_;
        Array<word> parent_;
        Array<BlockEntryInstr*> preorder_;
        Array<BlockEntryInstr*> postorder_;
        Array<BlockEntryInstr*> reverse_postorder_;

        void ComputeDominators(Array<BitVector*>* dominators);
    public:
        FlowGraph(Function* func, GraphEntryInstr* graph_entry):
                current_ssa_temp_index_(0),
                function_(func),
                graph_entry_(graph_entry),
                parent_(10),
                preorder_(10),
                postorder_(10),
                reverse_postorder_(10){}

        word AllocSSATempIndex(){
            return current_ssa_temp_index_++;
        }

        void AllocSSAIndexes(Definition* defn){
            defn->SetSSATempIndex(AllocSSATempIndex());
            AllocSSATempIndex();
        }

        GraphEntryInstr* GetEntry() const{
            return graph_entry_;
        }

        Definition* GetConstant(Instance* value);
        void ComputeSSA(word vreg);
        void DiscoverBlocks();
    };
}

#endif //ESCHELLE_CONTROL_FLOW_H