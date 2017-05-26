#ifndef ESCHELLE_AST_PRINTER_H
#define ESCHELLE_AST_PRINTER_H

#include "ast.h"

namespace Eschelle{
    class AstPrinter : public AstNodeVisitor{
    private:
        std::ostream& stream_;
        int offset_ = 0;

        inline void Adjust(){
            for(int i = 0; i < offset_; i++) std::cout << " ";
        }
    public:
        AstPrinter(std::ostream& stream):
                stream_(stream){}
        ~AstPrinter(){}

        void VisitSequence(SequenceNode* node){
            Adjust();
            stream_ << "{" << std::endl;
            offset_++;
            node->VisitChildren(this);
            offset_--;
            stream_ << "}" << std::endl;
        }

        void VisitLiteral(LiteralNode* node){
            stream_ << node->GetLiteral()->ToString();
        }

        void VisitBinaryOp(BinaryOpNode* node){
            node->GetLeft()->Visit(this);
            switch(node->GetKind()){
                case kPLUS: stream_ << " + "; break;
                case kMINUS: stream_ << " - "; break;
                case kDIVIDE: stream_ << " / "; break;
                case kMULTIPLY: stream_ << " * "; break;
                default: stream_ << " ? "; break;
            }
            node->GetRight()->Visit(this);
        }

        void VisitStoreLocal(StoreLocalNode* node){
            Adjust();
            stream_ << "SL " << node->GetLocal()->GetName();
            stream_ << "[" << node->GetLocal()->GetType()->GetName() << "] := ";
            node->VisitChildren(this);
            stream_ << std::endl;
        }

        void VisitStoreInstanceField(StoreInstanceFieldNode* node){
            Adjust();
            stream_ << "SI " << node->GetField()->GetName();
            stream_ << "[" << node->GetField()->GetFieldType()->GetName() << "] := ";
            node->VisitChildren(this);
            stream_ << std::endl;
        }

        void VisitStoreStaticField(StoreStaticFieldNode* node){
            Adjust();
            stream_ << "SS " << node->GetField()->GetName();
            stream_ << "[" << node->GetField()->GetFieldType()->GetName() << "] := ";
            node->VisitChildren(this);
            stream_ << std::endl;
        }

        void VisitReturn(ReturnNode* node){
            Adjust();
            stream_ << "return ";
            node->VisitChildren(this);
            stream_ << std::endl;
        }

        void VisitLoadLocal(LoadLocalNode* node){
            Adjust();
            stream_ << "LL " << node->GetLocal()->GetName()
                    << "[" << node->GetLocal()->GetType()->GetName() << "]";
        }

        void VisitLoadInstanceField(LoadInstanceFieldNode* node){
            Adjust();
            stream_ << "LI " << node->GetField()->GetName()
                    << "[" << node->GetField()->GetFieldType()->GetName() << "]";
        }

        void VisitLoadStaticField(LoadStaticFieldNode* node){
            Adjust();
            stream_ << "LS " << node->GetField()->GetName()
                    << "[" << node->GetField()->GetFieldType()->GetName() << "]";
        }
    };
}

#endif //ESCHELLE_AST_PRINTER_H
