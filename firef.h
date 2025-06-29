
#ifndef FIREF_H
#define FIREF_H 

#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 

#define FIREF_IMPL

#ifdef __cplusplus 
extern "C" {
#endif

// In Bytes
#define FR_STANDARD_FILE_SIZE 1024
#define FR_STANDARD_READ_SIZE_STEP 1024

// In Elements
#define FR_STANDARD_ARRAY_COUNT 200
#define FR_STANDARD_VERTICES_STEP 128
#define FR_MAX_OBJ_LINE_SIZE 128
#define FR_MAX_INDICIES 4 // no support for n-gons

typedef struct {
    unsigned int numVertices;
    unsigned int numUV;
    unsigned int numNormals;
    unsigned int numIndicies;

    unsigned int capacityVertices;
    unsigned int capacityUV;
    unsigned int capacityNormals;
    unsigned int capacityIndicies;

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
void fr_printVertices(fr_Obj* obj);
void fr_printUv(fr_Obj* obj);
void fr_printNormals(fr_Obj* obj);
void fr_printIndicies(fr_Obj* obj);;
void fr_printObj(fr_Obj* obj);

#ifdef __cplusplus
}
#endif

#endif // Firef_H

#ifdef FIREF_IMPL

void fr_loadObj(const char *filepath, fr_Obj *obj) {
    obj->vertices = (float*)malloc(FR_STANDARD_ARRAY_COUNT * sizeof(float));
    obj->uv = (float*)malloc(FR_STANDARD_ARRAY_COUNT * sizeof(float));
    obj->normals = (float*)malloc(FR_STANDARD_ARRAY_COUNT * sizeof(float));
    obj->indicies = (unsigned int*)malloc(FR_STANDARD_ARRAY_COUNT * sizeof(unsigned int));

    if (obj->vertices == NULL ||
        obj->uv == NULL ||
        obj->normals == NULL || 
        obj->indicies == NULL) {
        printf("[ERROR] Couldnt Allocate Default Memory for fr_Obj Arrays!\n");
        return;
    }

    obj->numVertices = 0;
    obj->numUV = 0;
    obj->numNormals = 0;
    obj->numIndicies = 0;

    obj->capacityVertices = sizeof(float) * FR_STANDARD_ARRAY_COUNT;
    obj->capacityUV = sizeof(float) * FR_STANDARD_ARRAY_COUNT;
    obj->capacityNormals = sizeof(float) * FR_STANDARD_ARRAY_COUNT;
    obj->capacityIndicies = sizeof(unsigned int) * FR_STANDARD_ARRAY_COUNT;

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
    
    while (*buffer != '\0') {
        // Generate Line
        char line[FR_MAX_OBJ_LINE_SIZE];
        int i = 0;
        while (*buffer != '\n' && *buffer != '\0') {
            line[i++] = *buffer++;
        }
        line[i] = '\0';
        
        if (i < 2) { buffer++; continue; }

        int lineIndex = 0;
        switch (line[lineIndex++]) {
            case 'v':
                switch (line[lineIndex++]) {
                    // Vertex Pos
                    case ' ': {
                        
                        // Skip whitespace after f 
                        while (line[lineIndex] == ' ') {
                            lineIndex++;
                        }

                        double vertex[3];
                        int vertexIndex = 0;
                        char temp[FR_MAX_OBJ_LINE_SIZE];
                        int j;
                        vertex_pos_loop:
                        j = 0;
                        for (; line[lineIndex] != ' ' && line[lineIndex] != '\0' && line[lineIndex] != '\n'; j++) {
                            temp[j] = line[lineIndex++];
                        }
                        temp[j] = '\0';
                        char* succ;
                        vertex[vertexIndex++] = strtod(temp, &succ);
                        lineIndex++;
                        if (vertexIndex < 3) { goto vertex_pos_loop; }
                        
                        if (obj->capacityVertices < (obj->numVertices + 3) * sizeof(typeof(obj->vertices[0])))  {
                            obj->vertices = (float*)realloc(obj->vertices, obj->capacityVertices += FR_STANDARD_VERTICES_STEP * sizeof(typeof(obj->vertices[0]))); 
                            if (obj->vertices == NULL) {
                                printf("[ERROR] Couldnt reallocate veritces buffer while parsing file!\n");
                                return;
                            }
                        }
                        obj->vertices[obj->numVertices++] = vertex[0];
                        obj->vertices[obj->numVertices++] = vertex[1];
                        obj->vertices[obj->numVertices++] = vertex[2];
                        break;
                    }
                        

                    // UV Cordinate
                    case 't': {

                        // Skip whitespace after f 
                        while (line[lineIndex] == ' ') {
                            lineIndex++;
                        }

                        double uv[2];
                        int uvIndex = 0;
                        char temp[FR_MAX_OBJ_LINE_SIZE];
                        int j;
                        uv_coord_loop:
                        j = 0;
                        for(; line[lineIndex] != ' ' && line[lineIndex] != '\0' && line[lineIndex] != '\n'; j++) {
                            temp[j] = line[lineIndex++];
                        }
                        temp[j] = '\0';
                        char* succ;
                        uv[uvIndex++] = strtod(temp, &succ);
                        lineIndex++;
                        if (uvIndex < 2) { goto uv_coord_loop; }

                        if (obj->capacityUV < (obj->numUV + 2) * sizeof(typeof(obj->uv[0]))) {
                            obj->uv = (float*)realloc(obj->uv, obj->capacityUV += (float)FR_STANDARD_VERTICES_STEP * sizeof(typeof(obj->uv[0])));
                            if (obj->uv == NULL) {
                                printf("[ERROR] Couldnt reallocate uv buffer while parsing file!\n");
                                return;
                            }
                        }
                        obj->uv[obj->numUV++] = uv[0];
                        obj->uv[obj->numUV++] = uv[1];
                        break;
                    }

                    // Normal
                    case 'n': {
        
                        // Skip whitespace after f 
                        while (line[lineIndex] == ' ') {
                            lineIndex++;
                        }

                        double normals[3];
                        int normalsIndex = 0;
                        char temp[FR_MAX_OBJ_LINE_SIZE];
                        int j;
                        normals_loop:
                        j = 0;
                        for(; line[lineIndex] != ' ' && line[lineIndex] != '\0' && line[lineIndex] != '\n'; j++) {
                            temp[j] = line[lineIndex++];
                        }
                        temp[j] = '\0';
                        char* succ;
                        normals[normalsIndex++] = strtod(temp, &succ);
                        lineIndex++;
                        if (normalsIndex < 3) { goto normals_loop; }

                        if (obj->capacityNormals < (obj->numNormals + 3) * sizeof(typeof(obj->normals[0]))) {
                            obj->normals = (float*)realloc(obj->normals, obj->capacityNormals += (float)FR_STANDARD_VERTICES_STEP * sizeof(typeof(obj->normals[0])));
                            if (obj->normals == NULL) {
                                printf("[ERROR] Couldnt reallocate normals buffer while parsing file!\n");
                                return;
                            }
                        }

                        obj->normals[obj->numNormals++] = normals[0];
                        obj->normals[obj->numNormals++] = normals[1];
                        obj->normals[obj->numNormals++] = normals[2];

                        break;
                    }
                }
                break;
            case 'f': {

                // Skip whitespace after f 
                while (line[lineIndex] == ' ') {
                    lineIndex++;
                }

                // v/vt/vn 
                // v == indicie
                unsigned int indicies[FR_MAX_INDICIES];
                int indiciesIndex = 0;

                while (1) {
                    
                    char temp[FR_MAX_OBJ_LINE_SIZE] = {0};
                    int j = 0;

                    while (line[lineIndex] != '/' && line[lineIndex] != ' ' &&
                            line[lineIndex] != '\0' && line[lineIndex] != '\n') {
                        temp[j++] = line[lineIndex++];
                    }

                    temp[j] = '\0';

                    char* succ;
                    indicies[indiciesIndex++] = strtoul(temp, &succ, 10) - 1;
              
                    // Skip the other things
                    while (line[lineIndex] != ' ' && line[lineIndex] != '\0' && line[lineIndex] != '\n') {
                        lineIndex++;
                    }

                    while (line[lineIndex] == ' ') lineIndex++;

                    // 3 Indicies
                    if (indiciesIndex == 3 && !(line[lineIndex] != '\0' && line[lineIndex] != '\n')) {
                        break;
                    }
                    // 4 Indicies
                    if (indiciesIndex == 4) { break; }

                }

                if (obj->capacityIndicies < (obj->numIndicies + FR_MAX_INDICIES) * sizeof(typeof(obj->indicies[0]))) {
                    obj->indicies = (unsigned int*)realloc(obj->indicies, obj->capacityIndicies += (float)FR_STANDARD_VERTICES_STEP * sizeof(typeof(obj->indicies[0])));
                    if (obj->indicies == NULL) {
                        printf("[ERROR] Couldnt reallocate normals buffer while parsing file!\n");
                        return;
                    }
                }
                
                if (indiciesIndex == 3) {
                    obj->indicies[obj->numIndicies++] = indicies[0];
                    obj->indicies[obj->numIndicies++] = indicies[1];
                    obj->indicies[obj->numIndicies++] = indicies[2];

                }
                else if (indiciesIndex == 4) {
                    // perform triangulation
                    obj->indicies[obj->numIndicies++] = indicies[0];
                    obj->indicies[obj->numIndicies++] = indicies[1];
                    obj->indicies[obj->numIndicies++] = indicies[2];

                    obj->indicies[obj->numIndicies++] = indicies[0];
                    obj->indicies[obj->numIndicies++] = indicies[2];
                    obj->indicies[obj->numIndicies++] = indicies[3];
                }
                else {
                    printf("[ERROR] Invalid Indicies Index!\n");
                }

                break;
            }
        }

        buffer++;
    }

}

void fr_printVertices(fr_Obj *obj) {
    printf("-- Vertices -- NumVertices: %d\n", obj->numVertices);
    for (int i = 0; i < obj->numVertices; i+=3) {
        printf("%d: %f", i/3, obj->vertices[i]);
        printf(" | %f", obj->vertices[i+1]);
        printf(" | %f\n", obj->vertices[i+2]);
    }
}

void fr_printUv(fr_Obj *obj) {
    printf("-- UV -- NumUv: %d\n", obj->numUV);
    for (int i = 0; i < obj->numUV; i+=2) {
        printf("%d: %f", i/2, obj->uv[i]);
        printf(" | %f\n", obj->uv[i+1]);
    }
}

void fr_printNormals(fr_Obj *obj) {
    printf("-- Normals -- NumNormals: %d\n", obj->numNormals);
    for (int i = 0; i < obj->numNormals; i+=3) {
        printf("%d: %f", i/3, obj->normals[i]);
        printf(" | %f", obj->normals[i+1]);
        printf(" | %f\n", obj->normals[i+2]);
    }
}

void fr_printIndicies(fr_Obj *obj) {
    printf("-- Indicies -- NumIndicies: %d\n", obj->numIndicies);
    for (int i = 0; i < obj->numIndicies; i+=3) {
        printf("%d: %d", i/3, obj->indicies[i]);
        printf(" | %d", obj->indicies[i+1]);
        printf(" | %d\n", obj->indicies[i+2]);
    }
}

void fr_printObj(fr_Obj *obj) {
    fr_printVertices(obj);
    printf("\n\n");
    fr_printUv(obj);
    printf("\n\n");
    fr_printNormals(obj);
    printf("\n\n");
    fr_printIndicies(obj);
}

#endif



