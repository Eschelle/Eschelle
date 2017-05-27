#include "flow_graph_builder.h"

namespace Eschelle{
    void EffectVisitor::VisitLiteral(LiteralNode *node){
        ReturnDefinition(new ConstantInstr(node->GetLiteral()));
    }

    void EffectVisitor::VisitSequence(SequenceNode *node){
        word i = 0;
        while(IsOpen() && (i < node->ChildrenSize())){
            EffectVisitor vis(Owner());
            node->GetChild(i++)->Visit(&vis);
            Append(vis);
            if(!IsOpen()){
                break;
            }
        }
    }

    void EffectVisitor::VisitReturn(ReturnNode *node){
        ValueVisitor for_value(Owner());
        node->Visit(&for_value);
        Append(for_value);
        AddReturnExit(for_value.GetValue());
    }

    void EffectVisitor::VisitStoreLocal(StoreLocalNode *node){
        ValueVisitor for_value(Owner());
        node->GetValue()->Visit(&for_value);
        Append(for_value);
        ReturnDefinition(new StoreLocalInstr(node->GetLocal(), for_value.GetValue()));
    }

    void EffectVisitor::VisitLoadLocal(LoadLocalNode *node){}

    void EffectVisitor::VisitBinaryOp(BinaryOpNode *node){
        ValueVisitor for_left_value(Owner());
        node->GetLeft()->Visit(&for_left_value);
        Append(for_left_value);

        ValueVisitor for_right_value(Owner());
        node->GetRight()->Visit(&for_right_value);
        Append(for_right_value);

        ReturnDefinition(new BinaryOpInstr(node->GetKind(), for_left_value.GetValue(), for_right_value.GetValue()));
    }

    void EffectVisitor::VisitLoadStaticField(LoadStaticFieldNode *node){

    }

    void EffectVisitor::VisitStoreStaticField(StoreStaticFieldNode *node){

    }

    void EffectVisitor::VisitLoadInstanceField(LoadInstanceFieldNode *node){

    }

    void EffectVisitor::VisitStoreInstanceField(StoreInstanceFieldNode *node){

    }

    void ValueVisitor::VisitLoadLocal(LoadLocalNode *node){
        Definition* defn;
        if(node->GetLocal()->IsConstant()){
            defn = new ConstantInstr(node->GetLocal()->GetConstantValue());
        } else{
            defn = new LoadLocalInstr(node->GetLocal());
        }
        ReturnDefinition(defn);
    }

    static Instruction* AppendFragment(BlockEntryInstr* entry, EffectVisitor vis){
        if(vis.IsEmpty()) return entry;
        Instruction::Link(entry, vis.GetEntry());
        return vis.GetExit();
    }

    FlowGraph* FlowGraphBuilder::BuildGraph(){
        TargetEntryInstr* normal = new TargetEntryInstr();
        entry_ = new GraphEntryInstr(function_, normal);

        EffectVisitor for_effect(this);
        ast_->Visit(&for_effect);
        AppendFragment(normal, for_effect);

        return new FlowGraph(function_, entry_);
    }
}