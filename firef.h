
#ifndef FIREF_H
#define FIREF_H 

#include <stdio.h>
#include <stdlib.h> 

#define FIREF_IMPL

#ifdef __cplusplus 
extern "C" {
#endif

#define FR_STANDARD_FILE_SIZE 1024
#define FR_STANDARD_READ_SIZE_STEP 1024
#define FR_MAX_OBJ_LINE_SIZE 128

typedef struct {
    float* vertices;
    float* uv;
    float* normals;
    unsigned int* indicies;
} fr_Obj;

void fr_loadObj(const char* filepath, fr_Obj* obj);
void fr_freeObj(fr_Obj* obj);

// internal functions
char* fr_readFile(const char* filepath, size_t* outBufferSize);
void fr_parseFile(char* buffer, fr_Obj* obj);

#ifdef __cplusplus
}
#endif

#endif // Firef_H

#ifdef FIREF_IMPL

void fr_loadObj(const char *filepath, fr_Obj *obj) {
    obj->vertices = NULL;
    obj->uv = NULL;
    obj->normals = NULL;
    obj->indicies = NULL;

    size_t bufferSize = 0;
    char* buffer = fr_readFile(filepath, &bufferSize);
    if (buffer == NULL) {
        printf("[ERROR] Couldnt read file: %s\n", filepath);
        printf("[ERROR] Couldnt construct ObjObject!\n");
        return;
    }
    if (bufferSize == 0) {
        printf("[ERROR] bufferSize when reading file: %s did not update!\n", filepath);
    }    

    fr_parseFile(buffer, obj);

}

void fr_freeObj(fr_Obj* obj) {
    if (obj->vertices != NULL)  free(obj->vertices);
    if (obj->uv != NULL)        free(obj->uv);
    if (obj->normals != NULL)   free(obj->normals);
    if (obj->indicies != NULL)  free(obj->indicies);
}

char* fr_readFile(const char* filepath, size_t* outBufferSize) {
    FILE* file = fopen(filepath, "rb");
    if (file == NULL) {
        printf("[ERROR] Opening File: %s\n", filepath);
        return NULL;
    }

    size_t capacity = FR_STANDARD_FILE_SIZE;
    size_t size = 0;

    char* buffer = (char*)malloc(sizeof(char) * capacity);
    if (buffer == NULL) {
        printf("[ERROR] Couldnt Allocate memory for reading file: %s\n", filepath);
        fclose(file);
        return NULL;
    }

    while (!feof(file)) {
        if (size + FR_STANDARD_READ_SIZE_STEP > capacity) {
            capacity += size + FR_STANDARD_READ_SIZE_STEP;
            char* newBuffer = (char*)realloc(buffer, capacity);
            if (newBuffer == NULL) {
                printf("[ERROR] Couldnt Allocate memory for reading file: %s\n", filepath);
                free(buffer);
                fclose(file);
                return NULL;
            }
            buffer = newBuffer;
        }

        size_t bytesRead = fread(buffer + size, sizeof(char), FR_STANDARD_READ_SIZE_STEP, file);
        size += bytesRead;

        if (bytesRead < FR_STANDARD_READ_SIZE_STEP) {
            if (ferror(file)) {
                printf("[ERROR] Error reading file: %s\n", filepath);
                perror("fread failed");
                free(buffer);
                fclose(file);
                return NULL;
            }
            break; // EOF or less then enough characters
        }
    }

    fclose(file);

    char* finalBuffer = (char*)realloc(buffer, size + 1);
    if (finalBuffer == NULL) {
        printf("[ERROR] Reallocating Final Buffer for Null terminating Character!\n");
        printf("[ERROR] Buffer from file: %s will not be Null terminated!\n", filepath);
        return buffer;
    }

    buffer = finalBuffer;
    buffer[size] = '\0';

    if (outBufferSize != NULL) {
        *outBufferSize = size;
    }
    else {
        printf("[ERROR] Writing Size into outBufferSize argument for file: %s\n", filepath);
    }

    return buffer;
}

void fr_parseFile(char *buffer, fr_Obj *obj) {
    
    int j = 0;
    while (*buffer != '\0') {
        char line[FR_MAX_OBJ_LINE_SIZE];
        int i = 0;
        while (*buffer != '\n' && *buffer != '\0') {
            line[i++] = *buffer++;
        }
        line[i] = '\0';
        printf("%d Line:  %s\n", j++, line);
        
        buffer++;
    }

}

#endif



