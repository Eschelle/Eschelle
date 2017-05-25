#include "flow_graph.h"

namespace Eschelle{
    Definition* FlowGraph::GetConstant(Instance *value){
        ConstantInstr* c = new ConstantInstr(value);
        c->SetSSATempIndex(AllocSSATempIndex());
        c->SetPrevious(graph_entry_);
        graph_entry_->GetDefinitions().Add(c);
        return c;
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
    }
}