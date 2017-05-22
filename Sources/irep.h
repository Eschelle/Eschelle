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

    class Instruction{
    private:
        Instruction* next_;
        Instruction* prev_;
    public:
        Instruction():
                next_(nullptr),
                prev_(nullptr){}

#define DECLARE_INSTRUCTION_TYPECHECK(Name) \
        bool Is##Name(){ return As##Name() != nullptr; } \
        virtual Name##Instr* As##Name(){ return nullptr; }
    FOR_EACH_INSTRUCTION(DECLARE_INSTRUCTION_TYPECHECK)
#undef DECLARE_INSTRUCTION_TYPECHECK
    };

    class BlockEntryInstr : public Instruction{
    private:
        word start_pos_;
        word end_pos_;
        Instruction* last_;
    protected:
        BlockEntryInstr():
                start_pos_(0),
                end_pos_(0),
                last_(nullptr){}

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
    };

    class GraphEntryInstr : public BlockEntryInstr{
    private:
        Function* function_;
    public:
        GraphEntryInstr(Function* function):
                function_(function){}
    };

    class TargetEntryInstr : public BlockEntryInstr{
    public:
        TargetEntryInstr(){}
    };

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
}

#endif //ESCHELLE_IREP_H