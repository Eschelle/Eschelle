#ifndef ESCHELLE_CONTROL_FLOW_H
#define ESCHELLE_CONTROL_FLOW_H

#include "common.h"
#include "object.h"

namespace Eschelle{
    class FlowGraph{
    private:
        Function* function_;
        word current_ssa_temp_index_;
    public:

        word GetCurrentSSATempIndex() const{
            return current_ssa_temp_index_;
        }
    };
}

#endif //ESCHELLE_CONTROL_FLOW_H