#ifndef ESCHELLE_IREP_H
#define ESCHELLE_IREP_H

#include "common.h"
#include "array.h"

namespace Eschelle{
#define FOR_EACH_INSTRUCTION(V) \
    V(GraphEntry) \
    V(TargetEntry) \
    V(JoinEntry) \
    V(ParallelMove) \
    V(Box) \
    V(Unbox) \
    V(Phi) \
    V(DropConstants) \
    V(Constant) \
    V(Return) \
    V(StoreLocal) \
    V(LoadLocal) \
    V(NativeCall)

    enum Representation{
        kUnboxedInt,
        kBoxedInt,

        kUnboxedDouble,
        kBoxedDouble,

        kTagged,
        kNone
    };

    class Definition;

    class Value{
    private:
        Value* prev_;
        Value* next_;
        Instruction* instruction_;
        Definition* definition_;
        word use_index_;
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
                if(next_ != nullptr) next_ = next_->GetNext();
            }
        };

        Value(Definition* defn):
                definition_(defn),
                prev_(nullptr),
                next_(nullptr),
                instruction_(nullptr),
                use_index_(nullptr){}

        Definition* GetDefinition() const{
            return definition_;
        }

        word UseIndex() const{
            return use_index_;
        }

        Value* GetNext() const{
            return next_;
        }

        void SetNext(Value* value){
            next_ = value;
        }

        Value* GetPrevious() const{
            return prev_;
        }

        void SetPrevious(Value* value){
            prev_ = value;
        }

        static void AddToList(Value* value, Value** list){
            Value* next = *list;
            *list = value;
            value->SetNext(next);
            value->SetPrevious(nullptr);
            if(next != nullptr) next->SetPrevious(value);
        }
    };

    class Instruction{
    private:
        word lifetime_pos_;
        Instruction* next_;
        Instruction* prev_;
    public:
        Instruction():
                lifetime_pos_(-1),
                prev_(nullptr),
                next_(nullptr){}
        virtual ~Instruction(){}

        virtual word InputCount() const = 0;
        virtual Value* InputAt(word i) const = 0;
    };

    class BlockEntryInstr : public Instruction{
    private:
        word preorder_num_;
        word postorder_num_;
        word start_pos_;
        word end_pos_;
        word offset_;
        Instruction* last_;
        Array<BlockEntryInstr*> dominated_;
        BlockEntryInstr* dominator_;
    };
}

#endif //ESCHELLE_IREP_H