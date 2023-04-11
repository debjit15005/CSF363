#ifndef PARSEDEF_H
#include "grammarDef.h"

struct scopeLim{
    int start;
    int end;
};
typedef struct scopeLim scopeLim;
typedef scopeLim* SLIM;
#endif