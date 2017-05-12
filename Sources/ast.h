#ifndef ESCHELLE_AST_H
#define ESCHELLE_AST_H

#include "common.h"
#include "object.h"
#include "scope.h"
#include "token.h"

namespace Eschelle{
#define FOR_EACH_NODE(V) \
    V(Return) \
    V(Literal) \
    V(Sequence) \
    V(BinaryOp) \
    V(StoreStaticField) \
    V(LoadStaticField)

#define DECLARE_COMMON_NODE_FUNCTIONS(BaseName) \
    virtual const char* Name(){ return #BaseName; } \
    virtual void Visit(AstNodeVisitor* vis); \
    virtual BaseName##Node* As##BaseName(){ return this; }

#define FORWARD_DECLARE(BaseName) class BaseName##Node;
    FOR_EACH_NODE(FORWARD_DECLARE)
#undef FORWARD_DECLARE

    class AstNodeVisitor{
    public:
        AstNodeVisitor(){}
        virtual ~AstNodeVisitor(){}

#define DECLARE_VISIT_FUNCTION(BaseName) \
        virtual void Visit##BaseName(BaseName##Node* node){}
    FOR_EACH_NODE(DECLARE_VISIT_FUNCTION)
#undef DECLARE_VISIT_FUNCTION
    };

    class AstNode{
    public:
        AstNode(){}
        virtual ~AstNode(){}

#define DEFINE_TYPE_CHECK(BaseName) \
        bool Is##BaseName(){ return As##BaseName() != nullptr; } \
        virtual BaseName##Node* As##BaseName(){ return nullptr; }
    FOR_EACH_NODE(DEFINE_TYPE_CHECK)
#undef DEFINE_TYPE_CHECK

        virtual const char* Name() = 0;
        virtual void Visit(AstNodeVisitor* vis) = 0;
        virtual void VisitChildren(AstNodeVisitor* vis) = 0;
    };

    class SequenceNode : public AstNode{
    private:
        Array<AstNode*> children_;
        LocalScope* scope_;
    public:
        SequenceNode():
                children_(10),
                scope_(nullptr){}

        void Add(AstNode* node){
            children_.Add(node);
        }

        void VisitChildren(AstNodeVisitor* vis){
            for(int i = 0; i < children_.Length(); i++){
                children_[i]->Visit(vis);
            }
        }

        DECLARE_COMMON_NODE_FUNCTIONS(Sequence)
    };

    class BinaryOpNode : public AstNode{
    private:
        TokenKind kind_;
        AstNode* left_;
        AstNode* right_;
    public:
        BinaryOpNode(AstNode* left, AstNode* right, TokenKind kind):
                kind_(kind),
                left_(left),
                right_(right){}
        ~BinaryOpNode(){}

        TokenKind  GetKind() const{
            return kind_;
        }

        AstNode* GetLeft() const{
            return left_;
        }

        AstNode* GetRight() const{
            return right_;
        }

        void VisitChildren(AstNodeVisitor* vis){

        }

        DECLARE_COMMON_NODE_FUNCTIONS(BinaryOp)
    };

    class ReturnNode : public AstNode{
    public:
        ReturnNode(){}
        ~ReturnNode(){}

        void VisitChildren(AstNodeVisitor* vis){}

        DECLARE_COMMON_NODE_FUNCTIONS(Return)
    };

    class LiteralNode : public AstNode{
    public:
        LiteralNode(){}
        ~LiteralNode(){}

        void VisitChildren(AstNodeVisitor* vis){

        }

        DECLARE_COMMON_NODE_FUNCTIONS(Literal)
    };

    class StoreStaticFieldNode : public AstNode{
    public:
        StoreStaticFieldNode(){}
        ~StoreStaticFieldNode(){}

        void VisitChildre(AstNodeVisitor* vis){

        }

        DECLARE_COMMON_NODE_FUNCTIONS(StoreStaticField)
    };

    class LoadStaticFieldNode : public AstNode{
    public:
        LoadStaticFieldNode(){}
        ~LoadStaticFieldNode(){}

        void VisitChildren(AstNodeVisitor* vis){

        }

        DECLARE_COMMON_NODE_FUNCTIONS(LoadStaticField)
    };
}

#endif //ESCHELLE_AST_H