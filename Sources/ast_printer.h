#ifndef ESCHELLE_AST_PRINTER_H
#define ESCHELLE_AST_PRINTER_H

#include "ast.h"

namespace Eschelle{
    class AstPrinter : public AstNodeVisitor{
    private:
        int offset_ = 0;
    public:
        AstPrinter(){}
        ~AstPrinter(){}

        void VisitSequence(SequenceNode* node){
            std::cout << node->Name() << std::endl;
            node->VisitChildren(this);
        }

        void VisitStoreStaticField(StoreStaticFieldNode* node){
            std::cout << node->Name() << std::endl;
            node->VisitChildren(this);
        }
    };
}

#endif //ESCHELLE_AST_PRINTER_H
