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
    V(LoadStaticField) \
    V(StoreLocal) \
    V(LoadLocal) \
    V(StoreInstanceField) \
    V(LoadInstanceField)

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

        virtual bool IsConstantExpr() const{
            return false;
        }

        virtual Instance* EvalConstantExpr() const{
            return nullptr;
        }
    };

    class SequenceNode : public AstNode{
    private:
        Array<AstNode*> children_;
        LocalScope* scope_;
    public:
        SequenceNode():
                children_(10),
                scope_(nullptr){}

        LocalScope* GetScope() const{
            return scope_;
        }

        void Add(AstNode* node){
            children_.Add(node);
        }

        void SetScope(LocalScope* scope){
            scope_ = scope;
        }

        void VisitChildren(AstNodeVisitor* vis){
            for(int i = 0; i < children_.Length(); i++){
                children_[i]->Visit(vis);
            }
        }

        AstNode* GetChild(word index) const{
            return children_[index];
        }

        word ChildrenSize() const{
            return children_.Length();
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
            GetLeft()->Visit(vis);
            GetRight()->Visit(vis);
        }

        DECLARE_COMMON_NODE_FUNCTIONS(BinaryOp)
    };

    class ReturnNode : public AstNode{
    private:
        AstNode* value_;
    public:
        ReturnNode(AstNode* value):
                value_(value){}
        ~ReturnNode(){}

        AstNode* GetValue() const{
            return value_;
        }

        void VisitChildren(AstNodeVisitor* vis){
            GetValue()->Visit(vis);
        }

        DECLARE_COMMON_NODE_FUNCTIONS(Return)
    };

    class LiteralNode : public AstNode{
    private:
        Instance* instance_;
    public:
        LiteralNode(Instance* instance):
            instance_(instance){}
        ~LiteralNode(){}

        Instance* GetLiteral() const{
            return instance_;
        }

        void VisitChildren(AstNodeVisitor* vis){}

        DECLARE_COMMON_NODE_FUNCTIONS(Literal)
    };

    class StoreInstanceFieldNode : public AstNode{
    private:
        Field* field_;
        AstNode* value_;
    public:
        StoreInstanceFieldNode(Field* field, AstNode* value):
                field_(field),
                value_(value){}

        Field* GetField() const{
            return field_;
        }

        AstNode* GetValue() const{
            return value_;
        }

        void VisitChildren(AstNodeVisitor* vis){
            GetValue()->Visit(vis);
        }

        DECLARE_COMMON_NODE_FUNCTIONS(StoreInstanceField);
    };

    class LoadInstanceFieldNode : public AstNode{
    private:
        Field* field_;
    public:
        LoadInstanceFieldNode(Field* field):
                field_(field){}

        Field* GetField() const{
            return field_;
        }

        void VisitChildren(AstNodeVisitor* vis){}

        DECLARE_COMMON_NODE_FUNCTIONS(LoadInstanceField);
    };

    class StoreStaticFieldNode : public AstNode{
    private:
        Field* field_;
        AstNode* value_;
    public:
        StoreStaticFieldNode(Field* field, AstNode* value):
            field_(field),
            value_(value){}
        ~StoreStaticFieldNode(){}

        Field* GetField() const{
            return field_;
        }

        AstNode* GetValue() const{
            return value_;
        }

        void VisitChildren(AstNodeVisitor* vis){
            GetValue()->Visit(vis);
        }

        DECLARE_COMMON_NODE_FUNCTIONS(StoreStaticField)
    };

    class LoadStaticFieldNode : public AstNode{
    private:
        Field* field_;
    public:
        LoadStaticFieldNode(Field* field):
                field_(field){}
        ~LoadStaticFieldNode(){}

        Field* GetField() const{
            return field_;
        }

        void VisitChildren(AstNodeVisitor* vis){}

        DECLARE_COMMON_NODE_FUNCTIONS(LoadStaticField)
    };

    class StoreLocalNode : public AstNode{
    private:
        LocalVariable* local_;
        AstNode* value_;
    public:
        StoreLocalNode(LocalVariable* local, AstNode* value):
                local_(local),
                value_(value){}
        ~StoreLocalNode(){}

        LocalVariable* GetLocal() const{
            return local_;
        }

        AstNode* GetValue() const{
            return value_;
        }

        void VisitChildren(AstNodeVisitor* vis){
            GetValue()->Visit(vis);
        }

        DECLARE_COMMON_NODE_FUNCTIONS(StoreLocal);
    };

    class LoadLocalNode : public AstNode{
    private:
        LocalVariable* local_;
    public:
        LoadLocalNode(LocalVariable* local):
                local_(local){}
        ~LoadLocalNode(){}

        LocalVariable* GetLocal() const{
            return local_;
        }

        void VisitChildren(AstNodeVisitor* vis){}

        DECLARE_COMMON_NODE_FUNCTIONS(LoadLocal);
    };
}

#endif //ESCHELLE_AST_H