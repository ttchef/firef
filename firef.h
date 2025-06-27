
#ifndef FIREF_H
#define FIREF_H 

#include <stdlib.h> 

#ifdef __cplusplus 
extern "C" {
#endif

typedef struct {
    float* vertices;
    float* uv;
    float* normals;
    unsigned int* indicies;
} fr_ObjObject;

void fr_loadObjObject(const char* filepath, fr_ObjObject* obj);
void fr_freeObjObject(fr_ObjObject* obj);

#ifdef __cplusplus
}
#endif

#endif // Firef_H

#ifdef FIREF_IMPL

void fr_loadObjObject(const char *filepath, fr_ObjObject *obj) {
    
}

void fr_freeObjObject(fr_ObjObject* obj) {
    free(obj->vertices);
    free(obj->uv);
    free(obj->normals);
    free(obj->indicies);
}

#endif



