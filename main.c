
#include <stdio.h> 

#define FIREF_IMPL
#include "firef.h"

int main() {

    fr_Obj cube;
    fr_loadObj("cube.obj", &cube);

    // for debugging
    fr_printObj(&cube);

    fr_ArrayFloat test = fr_mergeArrays(&cube);
    fr_printArrayFloatMoreInfo(&test, &cube);

    fr_freeArrayFloat(&test);
    fr_freeObj(&cube);

    return 0;
}


