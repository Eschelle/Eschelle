#include "flow_graph.h"

namespace Eschelle{
    Definition* FlowGraph::GetConstant(Instance *value){
        /*
        ConstantInstr* c = new ConstantInstr(value);
        c->SetSSATempIndex(AllocSSATempIndex());
        c->SetPrevious(graph_entry_);
        graph_entry_->GetDefinitions().Add(c);
        return c;
         */
        return nullptr;
    }

    struct BlockTraversalState{
        BlockEntryInstr* block;
        word successor;

        BlockTraversalState(BlockEntryInstr* b, word s):
                block(b),
                successor(s){}
    };

    void FlowGraph::DiscoverBlocks(){
        preorder_.Clear();
        postorder_.Clear();
        reverse_postorder_.Clear();

        Array<BlockTraversalState> block_stack(10);
        graph_entry_->DiscoverBlocks(nullptr, &preorder_, &parent_);

        BlockTraversalState init_state(graph_entry_, graph_entry_->SuccessorCount() - 1);
        block_stack.Add(init_state);

        while(!block_stack.IsEmpty()){
            BlockTraversalState state = block_stack.Last();

            if(state.successor >= 0){
                BlockEntryInstr* successor = state.block->GetLastInstruction()->SuccessorAt(state.successor++);
                if(successor->DiscoverBlocks(state.block, &preorder_, &parent_)){
                    block_stack.Add(BlockTraversalState(successor, successor->GetLastInstruction()->SuccessorCount() - 1));
                }
            } else{
                block_stack.Pop();
                state.block->postorder_num_ = postorder_.Length();
                postorder_.Add(state.block);
            }
        }

        word count = postorder_.Length();
        for(int i = 0; i < count; i++) reverse_postorder_.Add(postorder_[count - i - 1]);
    }

    void FlowGraph::ComputeSSA(word vreg){
        current_ssa_temp_index_ = vreg;
    }
}