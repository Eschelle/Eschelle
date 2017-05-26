#ifndef ESCHELLE_IREP_H
#define ESCHELLE_IREP_H

#include "common.h"
#include "array.h"
#include "object.h"

namespace Eschelle{
#define FOR_EACH_INSTRUCTION(V) \
    V(GraphEntry) \
    V(TargetEntry) \
    V(Box) \
    V(Unbox) \
    V(Constant) \
    V(Return) \
    V(BinaryOp) \

#define FOR_EACH_ABSTRACT_INSTRUCTION(V) \
    V(BlockEntry)

#define FORWARD_DECLARE(Name) \
    class Name##Instr;
    FOR_EACH_INSTRUCTION(FORWARD_DECLARE)
    FOR_EACH_ABSTRACT_INSTRUCTION(FORWARD_DECLARE)
#undef FORWARD_DECLARE

    class Definition;

    class Instruction{
    private:
        Instruction* next_;
        Instruction* prev_;
    public:
        Instruction():
                next_(nullptr),
                prev_(nullptr){}

        Instruction* GetNext() const{
            return next_;
        }

        void SetNext(Instruction* next){
            next_ = next;
        }

        Instruction* GetPrevious() const{
            return prev_;
        }

        void SetPrevious(Instruction* prev){
            prev_ = prev;
        }

        virtual word SuccessorCount() const = 0;
        virtual BlockEntryInstr* SuccessorAt(word index) const = 0;

#define DECLARE_INSTRUCTION_TYPECHECK(Name) \
        bool Is##Name(){ return As##Name() != nullptr; } \
        virtual Name##Instr* As##Name(){ return nullptr; }
    FOR_EACH_INSTRUCTION(DECLARE_INSTRUCTION_TYPECHECK)
#undef DECLARE_INSTRUCTION_TYPECHECK
    };

    class BlockEntryInstr : public Instruction{
    private:
        word preorder_num_;
        word postorder_num_;
        word offset_;
        word start_pos_;
        word end_pos_;
        Instruction* last_;
        Array<BlockEntryInstr*> dominated_;
        BlockEntryInstr* dominator_;

        friend class FlowGraph;
    protected:
        BlockEntryInstr():
                preorder_num_(0),
                postorder_num_(0),
                offset_(0),
                start_pos_(0),
                end_pos_(0),
                dominator_(nullptr),
                dominated_(10),
                last_(nullptr){}
    public:
        void SetStartPos(word pos){
            start_pos_ = pos;
        }

        word GetStartPos() const{
            return start_pos_;
        }

        void SetEndPos(word pos){
            end_pos_ = pos;
        }

        word GetEndPos() const{
            return end_pos_;
        }

        word GetPreorderNum() const{
            return preorder_num_;
        }

        void SetPreorderNum(word preorder){
            preorder_num_ = preorder;
        }

        word GetPostorderNum() const{
            return postorder_num_;
        }

        void SetPostorderNum(word postorder){
            postorder_num_ = postorder;
        }

        void SetLastInstruction(Instruction* last){
            last_ = last;
        }

        Instruction* GetLastInstruction () const{
            return last_;
        }

        virtual word GetPredecessorsCount() const = 0;
        virtual void ClearPredecessors() = 0;
        virtual void AddPredecessor(BlockEntryInstr* predecessor) = 0;
        virtual BlockEntryInstr* GetPredecessorAt(word index) const = 0;

        bool DiscoverBlocks(BlockEntryInstr* predecessor, Array<BlockEntryInstr*>* preorder, Array<word>* parent);
    };

    class GraphEntryInstr : public BlockEntryInstr{
    private:
        Function* function_;
        TargetEntryInstr* block_;
        word entry_count_;
        word spill_slot_count_;
        word fixed_slot_count_;
        Array<Definition*> definitions_;
    public:
        GraphEntryInstr(Function* function, TargetEntryInstr* target):
                block_(target),
                entry_count_(0),
                spill_slot_count_(0),
                fixed_slot_count_(0),
                definitions_(10),
                function_(function){}

        Array<Definition*> GetDefinitions() const{
            return definitions_;
        }

        void ClearPredecessors(){}
        void AddPredecessor(BlockEntryInstr* block){}

        BlockEntryInstr* GetPredecessorAt(word index) const{
            return nullptr;
        }

        word GetPredecessorsCount() const{
            return 0;
        }

        word SuccessorCount() const{
            return 1;
        }

        BlockEntryInstr* SuccessorAt(word index) const;
    };

    class TargetEntryInstr : public BlockEntryInstr{
    private:
        BlockEntryInstr* predecessor_;
    public:
        TargetEntryInstr():
                predecessor_(nullptr){}

        void AddPredecessor(BlockEntryInstr* block){
            predecessor_ = block;
        }

        BlockEntryInstr* GetPredecessorAt(word index) const{
            return predecessor_;
        }

        void ClearPredecessors(){
            predecessor_ = nullptr;
        }

        word GetPredecessorsCount() const{
            return 1;
        }

        word SuccessorCount() const{
            return 0;
        }

        BlockEntryInstr* SuccessorAt(word index) const{
            return nullptr;
        }
    };

    class Value{
    private:
        Definition* defn_;
        word use_index_;
        Value* next_use_;
        Value* previous_use_;
    public:
        class Iterator{
        private:
            Value* current_;
            Value* next_;
        public:
            Iterator(Value* head):
                    next_(head){
                Advance();
            }

            Value* Current() const{
                return current_;
            }

            bool Done() const{
                return current_ == nullptr;
            }

            void Advance(){
                current_ = next_;
                if(next_ != nullptr){
                    next_ = next_->NextUse();
                }
            }
        };

        Value(Definition* defn):
                defn_(defn),
                previous_use_(nullptr),
                next_use_(nullptr),
                use_index_(-1){}

        Value* NextUse() const{
            return next_use_;
        }

        void SetNextUse(Value* value){
            next_use_ = value;
        }

        Value* PreviousUse() const{
            return previous_use_;
        }

        void SetPreviousUse(Value* value){
            previous_use_ = value;
        }

        word UseIndex() const{
            return use_index_;
        }

        void SetUseIndex(word index){
            use_index_ = index;
        }
    };

    class Definition : public Instruction{
    private:
        word temp_index_;
        word ssa_temp_index_;
    public:
        void SetSSATempIndex(word index){
            ssa_temp_index_ = index;
        }

        word GetSSATempIndex() const{
            return ssa_temp_index_;
        }

        void SetTempIndex(word index){
            temp_index_ = index;
        }

        word GetTempIndex() const{
            return temp_index_;
        }

        word SuccessorCount() const{
            return 0;
        }

        BlockEntryInstr* GetSuccessorAt(word index) const{
            return nullptr;
        }
    };

    class ConstantInstr : public Definition{
    private:
        Instance* value_;
    public:
        ConstantInstr(Instance* value):
                value_(value){}

        Instance* GetValue() const{
            return value_;
        }
    };
}

#endif //ESCHELLE_IREP_H