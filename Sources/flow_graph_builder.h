#ifndef ESCHELLE_FLOW_GRAPH_BUILDER_H
#define ESCHELLE_FLOW_GRAPH_BUILDER_H

#include "common.h"
#include "flow_graph.h"
#include "ast.h"

namespace Eschelle{
    class FlowGraphBuilder{
    private:
        AstNode* ast_;
        GraphEntryInstr* entry_;
        word temp_count_;
        Function* function_;

    protected:
        inline word AllocTemp(){
            return temp_count_++;
        }

        inline void DeallocTemps(word count){
            temp_count_ -= count;
        }

        friend class EffectVisitor;
    public:
        FlowGraphBuilder(Function* func):
                function_(func),
                temp_count_(0),
                entry_(nullptr),
                ast_(func->GetAst()){}

        FlowGraph* BuildGraph();
    };

    class EffectVisitor : public AstNodeVisitor{
    private:
        FlowGraphBuilder* owner_;
        Instruction* exit_;
        Instruction* entry_;
    protected:
        void Append(EffectVisitor other){
            if(other.IsEmpty()) return;
            if(IsEmpty()){
                entry_ = other.entry_;
            } else{
                Instruction::Link(exit_, other.entry_);
            }
            exit_ = other.exit_;
        }

        void Do(Definition* defn){
            owner_->DeallocTemps(defn->GetInputCount());
            if(IsEmpty()){
                entry_ = defn;
            } else{
                Instruction::Link(exit_, defn);
            }
            exit_ = defn;
        }

        void Add(Instruction* instr){
            owner_->DeallocTemps(instr->GetInputCount());
            if(IsEmpty()){
                entry_ = exit_ = instr;
            } else{
                Instruction::Link(exit_, instr);
                exit_ = instr;
            }
        }

        void AddReturnExit(Value* value){
            Add(new ReturnInstr(value));
            exit_ = nullptr;
        }

        Value* Bind(Definition* defn){
            Owner()->DeallocTemps(defn->GetInputCount());
            defn->SetTempIndex(Owner()->AllocTemp());
            if(IsEmpty()){
                entry_ = defn;
            } else{
                Instruction::Link(exit_, defn);
            }
            exit_ = defn;
            return new Value(defn);
        }
    public:
        EffectVisitor(FlowGraphBuilder* owner):
                owner_(owner),
                exit_(nullptr),
                entry_(nullptr){}
        virtual ~EffectVisitor(){}

        bool IsEmpty() const{
            return entry_ == nullptr;
        }

        bool IsOpen() const{
            return IsEmpty() || exit_ != nullptr;
        }

        virtual void ReturnDefinition(Definition* defn){
            if(!defn->IsConstant()){
                Do(defn);
            }
        }

        FlowGraphBuilder* Owner() const{
            return owner_;
        }

        Instruction* GetEntry() const{
            return entry_;
        }

        Instruction* GetExit() const{
            return exit_;
        }

        virtual void ReturnValue(Value* value){}

#define DECLARE_VISIT_FUNCTION(Name) \
        virtual void Visit##Name(Name##Node* node);
FOR_EACH_NODE(DECLARE_VISIT_FUNCTION);
#undef DECLARE_VISIT_FUNCTION
    };

    class ValueVisitor : public EffectVisitor{
    private:
        Value* value_;
    public:
        ValueVisitor(FlowGraphBuilder* owner):
                value_(nullptr),
                EffectVisitor(owner){}
        virtual ~ValueVisitor(){}

        virtual void ReturnValue(Value* value){
            value_ = value;
        }

        virtual void ReturnDefinition(Definition* defn){
            value_ = Bind(defn);
        }

        Value* GetValue() const{
            return value_;
        }

        virtual void VisitLoadLocal(LoadLocalNode* node);
    };
}

#endif //ESCHELLE_FLOW_GRAPH_BUILDER_H
