#ifndef ESCHELLE_IREP_H
#define ESCHELLE_IREP_H

#include "common.h"
#include "array.h"
#include "object.h"
#include "scope.h"
#include "token.h"

namespace Eschelle{
#define FOR_EACH_INSTRUCTION(V) \
    V(GraphEntry) \
    V(TargetEntry) \
    V(Constant) \
    V(Return) \
    V(BinaryOp) \
    V(StoreLocal) \
    V(LoadLocal)

#define FOR_EACH_ABSTRACT_INSTRUCTION(V) \
    V(BlockEntry)

#define FORWARD_DECLARE(Name) \
    class Name##Instr;
    FOR_EACH_INSTRUCTION(FORWARD_DECLARE)
    FOR_EACH_ABSTRACT_INSTRUCTION(FORWARD_DECLARE)
#undef FORWARD_DECLARE

#define DECLARE_INSTRUCTION(Instr) \
    virtual const char* Name() const{ return #Instr; }

    class Definition;

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

        virtual word GetInputCount() const{
            return 0;
        }

        virtual Value* GetInputAt(word index) const{
            return nullptr;
        }

        virtual word SuccessorCount() const = 0;
        virtual BlockEntryInstr* SuccessorAt(word index) const = 0;

#define DECLARE_INSTRUCTION_TYPECHECK(Name) \
        bool Is##Name(){ return As##Name() != nullptr; } \
        virtual Name##Instr* As##Name(){ return nullptr; }
    FOR_EACH_INSTRUCTION(DECLARE_INSTRUCTION_TYPECHECK)
#undef DECLARE_INSTRUCTION_TYPECHECK

        virtual const char* Name() const = 0;

        static void Link(Instruction* one, Instruction* two){
            one->next_ = two;
            if(two != nullptr) two->prev_ = one;
        }
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

        Array<BlockEntryInstr*> GetDominated() const{
            return dominated_;
        }

        void AddDominated(BlockEntryInstr* block){
            dominated_.Add(block);
            block->dominator_ = this;
        }

        BlockEntryInstr* GetDominator() const{
            return dominator_;
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

        TargetEntryInstr* GetTarget() const{
            return block_;
        }

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

        DECLARE_INSTRUCTION(GraphEntry);
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

        DECLARE_INSTRUCTION(TargetEntry);
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

        BlockEntryInstr* SuccessorAt(word index) const{
            return nullptr;
        }

        virtual const char* Name() const = 0;
    };

    template<word N>
    class TemplateDefinition : public Definition{
    public:
        TemplateDefinition():
                Definition(){}
        virtual ~TemplateDefinition(){}

        virtual word GetInputCount() const{
            return N;
        }
    };

    class ConstantInstr : public TemplateDefinition<0>{
    private:
        Instance* value_;
    public:
        ConstantInstr(Instance* value):
                value_(value){}

        Instance* GetValue() const{
            return value_;
        }

        DECLARE_INSTRUCTION(Constant);
    };

    class ReturnInstr : public TemplateDefinition<1>{
    private:
        Value* input_;
    public:
        ReturnInstr(Value* input):
                input_(input),
                TemplateDefinition(){}

        virtual Value* GetInputAt(word index) const{
            return input_;
        }

        DECLARE_INSTRUCTION(Return);
    };

    class LoadLocalInstr : public TemplateDefinition<0>{
    private:
        LocalVariable* local_;
        bool is_last_;
    public:
        LoadLocalInstr(LocalVariable* local):
                local_(local),
                is_last_(false),
                TemplateDefinition(){}

        DECLARE_INSTRUCTION(LoadLocal);
    };

    class StoreLocalInstr : public TemplateDefinition<1>{
    private:
        LocalVariable* local_;
        bool is_dead_;
        bool is_last_;
        Value* value_;
    public:
        StoreLocalInstr(LocalVariable* local, Value* value):
                local_(local),
                value_(value),
                is_last_(false),
                is_dead_(false),
                TemplateDefinition(){}

        DECLARE_INSTRUCTION(StoreLocal);
    };

    class BinaryOpInstr : public TemplateDefinition<2>{
    private:
        Array<Value*> inputs_;
        TokenKind operation_;
    public:
        BinaryOpInstr(TokenKind oper, Value* left, Value* right):
                operation_(oper),
                inputs_(2),
                TemplateDefinition(){
            inputs_.Add(left);
            inputs_.Add(right);
        }

        virtual Value* GetInputAt(word index) const{
            return inputs_[index];
        }

        DECLARE_INSTRUCTION(BinaryOp);
    };
}

#endif //ESCHELLE_IREP_H