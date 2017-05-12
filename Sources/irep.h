#ifndef ESCHELLE_IREP_H
#define ESCHELLE_IREP_H

#include "common.h"

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
}

#endif //ESCHELLE_IREP_H