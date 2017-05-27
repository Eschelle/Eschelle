#include "irep.h"

namespace Eschelle{
    static inline bool IsMarked(BlockEntryInstr* block, Array<BlockEntryInstr*>* preorder){
        word index = block->GetPreorderNum();
        return index >= 0 &&
               index < preorder->Length() &&
                (*preorder)[index] == block;
    }

    bool BlockEntryInstr::DiscoverBlocks(BlockEntryInstr *predecessor, Array<BlockEntryInstr *> *preorder, Array<word> *parent){
        if(IsMarked(this, preorder)){
            AddPredecessor(predecessor);
            return false;
        }

        ClearPredecessors();
        if(predecessor != nullptr) AddPredecessor(predecessor);

        word parent_num = predecessor == nullptr ?
                          -1 :
                          predecessor->GetPreorderNum();
        parent->Add(parent_num);
        SetPreorderNum(preorder->Length());
        preorder->Add(this);

        Instruction* next = SuccessorAt(0);
        while(next->GetNext() != nullptr) next = next->GetNext();
        SetLastInstruction(next);
        return true;
    }

    BlockEntryInstr* GraphEntryInstr::SuccessorAt(word index) const{
        return block_;
    }
}