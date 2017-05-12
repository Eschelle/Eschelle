#include "ast.h"

namespace Eschelle{
#define DEFINE_VISIT_FUNCTION(BaseName) \
    void BaseName##Node::Visit(AstNodeVisitor* vis){ \
        vis->Visit##BaseName(this); \
    }
FOR_EACH_NODE(DEFINE_VISIT_FUNCTION)
#undef DEFINE_VISIT_FUNCTION
}