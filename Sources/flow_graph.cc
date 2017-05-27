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
                BlockEntryInstr* successor = state.block->GetLastInstruction()->SuccessorAt(state.successor--);
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

        Array<BitVector*> dominance(10);
        ComputeDominators(&dominance);
    }

    static inline word Minimum(word a, word b){
        return a < b ? a : b;
    }

    static inline void CompressPath(word start, word current, Array<word>* parent, Array<word>* label){
        word next = (*parent)[current];
        if(next > start){
            CompressPath(start, next, parent, label);
            label[current] = Minimum((*label)[current], (*label)[next]);
            parent[current] = parent[next];
        }
    }

    void FlowGraph::ComputeDominators(Array<BitVector *> *dominators){
        word size = parent_.Length();

        Array<word> idom(size);
        Array<word> semi(size);
        Array<word> label(size);

        for(int i = 0; i < size; i++){
            idom.Add(parent_[i]);
            semi.Add(i);
            label.Add(i);
            dominators->Add(new BitVector(size));
        }

        preorder_[0]->GetDominated().Clear();

        for(word block_index = size - 1; block_index >= 1; block_index--){
            BlockEntryInstr* block = preorder_[block_index];
            block->GetDominated().Clear();

            for(word i = 0; i < block->GetPredecessorsCount(); i++){
                BlockEntryInstr* predecessor = block->GetPredecessorAt(i);
                word pred_index = predecessor->GetPreorderNum();
                word best = pred_index;

                if(pred_index > block_index){
                    CompressPath(block_index, pred_index, &parent_, &label);
                }

                semi[block_index] = Minimum(semi[block_index], semi[best]);
            }

            label[block_index] = semi[block_index];
        }

        for(word block_index = 1; block_index < size; block_index++){
            word dom_index = idom[block_index];
            while(dom_index > semi[block_index]){
                dom_index = idom[dom_index];
            }
            idom[block_index] = dom_index;
            preorder_[dom_index]->AddDominated(preorder_[block_index]);
        }

        for(word block_index = 0; block_index < size; block_index++){
            BlockEntryInstr* block = preorder_[block_index];
            word count = block->GetPredecessorsCount();
            if(count <= 1) continue;
            for(word i = 0; i < count; i++){
                BlockEntryInstr* runner = block->GetPredecessorAt(block_index);
                while(runner != block->GetDominator()){
                    (*dominators)[runner->preorder_num_]->Add(block_index);
                    runner = runner->GetDominator();
                }
            }
        }
    }
}