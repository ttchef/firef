
#ifndef FIREF_H
#define FIREF_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FIREF_IMPL

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float *vertices;
    size_t vertex_count;

    unsigned int *indices;
    size_t index_count;
} Obj;

static inline float parse_float(const char *s) {
    return strtof(s, NULL);
}

static inline int parse_int(const char *s) {
    return strtol(s, NULL, 10);
}

#ifdef __cplusplus
}
#endif 

#endif

#ifdef FIREF_IMPL

Obj load_obj(const char *path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "Failed to open %s\n", path);
        exit(1);
    }

    float *positions = NULL, *uvs = NULL, *normals = NULL;
    size_t pos_len = 0, uv_len = 0, norm_len = 0;
    float *vertices = NULL;
    size_t vert_len = 0, vert_cap = 0;
    unsigned int *indices = NULL;
    size_t idx_len = 0, idx_cap = 0;

    char line[512];
    unsigned int vertex_counter = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "v ", 2) == 0 || strncmp(line, "vt", 2) == 0 || strncmp(line, "vn", 2) == 0) {
            float x = 0, y = 0, z = 0;
            if (line[1] == ' ') {
                sscanf(line + 2, "%f %f %f", &x, &y, &z);
                float *tmp = (float*)realloc(positions, (pos_len + 3) * sizeof(float));
                if (!tmp) exit(1);
                positions = tmp;
                positions[pos_len++] = x;
                positions[pos_len++] = y;
                positions[pos_len++] = z;
            } else if (line[1] == 't') {
                sscanf(line + 3, "%f %f", &x, &y);
                float *tmp = (float*)realloc(uvs, (uv_len + 2) * sizeof(float));
                if (!tmp) exit(1);
                uvs = tmp;
                uvs[uv_len++] = x;
                uvs[uv_len++] = y;
            } else if (line[1] == 'n') {
                sscanf(line + 3, "%f %f %f", &x, &y, &z);
                float *tmp = (float*)realloc(normals, (norm_len + 3) * sizeof(float));
                if (!tmp) exit(1);
                normals = tmp;
                normals[norm_len++] = x;
                normals[norm_len++] = y;
                normals[norm_len++] = z;
            }
        } else if (strncmp(line, "f ", 2) == 0) {
           char *token = strtok(line + 2, " \n");
            unsigned int face[32]; // Max 32 vertices per face
            int count = 0;

            // Schleife durch alle Vertex-Referenzen (z.B. "1/1/1", "2/2/2", "3/3/3") in einer Face-Zeile
            while (token && count < 32) {
                int current_vi = -1;
                int current_ti = -1;
                int current_ni = -1;

                char* p_token = token;
                char* end_ptr;

                // 1. Vertex Index (v) parsen
                current_vi = strtol(p_token, &end_ptr, 10);
                if (p_token == end_ptr) { // Konnte keine Zahl parsen
                    fprintf(stderr, "Error parsing vertex index in face line: %s\n", token);
                    exit(1);
                }
                p_token = end_ptr; // Zeiger nach der geparsten Zahl bewegen

                // 2. Ersten Slash prüfen und Texturkoordinaten-Index (vt) parsen
                if (*p_token == '/') {
                    p_token++; // Den ersten Slash 'konsumieren'
                    
                    if (*p_token != '/') { // Nur parsen, wenn es NICHT ein doppelter Slash ist (z.B. "v//vn")
                        current_ti = strtol(p_token, &end_ptr, 10);
                        p_token = end_ptr; // Zeiger nach der geparsten Zahl bewegen
                    }
                    
                    // 3. Zweiten Slash prüfen und Normalen-Index (vn) parsen
                    if (*p_token == '/') { // KORREKTUR: Hier muss auf '/' geprüft werden!
                        p_token++; // Den zweiten Slash 'konsumieren'
                        current_ni = strtol(p_token, &end_ptr, 10);
                        // p_token = end_ptr; // Nicht zwingend nötig, da letzter Teil des Tokens
                    }
                }
                
                // Indizes auf 0-basiert umwandeln
                // (Nur dekrementieren, wenn der Index >= 1 ist, da -1 bedeutet, dass er fehlte)
                if (current_vi > 0) current_vi--; 
                if (current_ti > 0) current_ti--; 
                if (current_ni > 0) current_ni--; 

                // Diese lokalen Variablen sind nicht mehr nötig, da wir direkt mit current_vi/ti/ni arbeiten
                // int vi = current_vi;
                // int ti = current_ti;
                // int ni = current_ni;

                // Daten aus den Arrays holen
                float vx = positions[current_vi * 3 + 0];
                float vy = positions[current_vi * 3 + 1];
                float vz = positions[current_vi * 3 + 2];

                float tx = (current_ti >= 0 && (size_t)current_ti * 2 + 1 < uv_len) ? uvs[current_ti * 2 + 0] : 0.0f;
                float ty = (current_ti >= 0 && (size_t)current_ti * 2 + 1 < uv_len) ? uvs[current_ti * 2 + 1] : 0.0f;

                // KORREKTUR: Fallback-Normalen auf (0,0,0) ändern, um die reine Weiß-Darstellung zu vermeiden
                float nx = (current_ni >= 0 && (size_t)current_ni * 3 + 2 < norm_len) ? normals[current_ni * 3 + 0] : 0.0f;
                float ny = (current_ni >= 0 && (size_t)current_ni * 3 + 2 < norm_len) ? normals[current_ni * 3 + 1] : 0.0f; // <-- GEÄNDERT!
                float nz = (current_ni >= 0 && (size_t)current_ni * 3 + 2 < norm_len) ? normals[current_ni * 3 + 2] : 0.0f; // <-- GEÄNDERT!

                // Speicher für vertices erweitern, falls nötig
                if (vert_len + 8 > vert_cap) {
                    size_t new_cap = vert_cap == 0 ? 64 : vert_cap * 2;
                    float *tmp = (float*)realloc(vertices, new_cap * sizeof(float));
                    if (!tmp) exit(1);
                    vertices = tmp;
                    vert_cap = new_cap;
                }

                // Interleaved Vertexdaten hinzufügen
                vertices[vert_len++] = vx;
                vertices[vert_len++] = vy;
                vertices[vert_len++] = vz;
                vertices[vert_len++] = tx;
                vertices[vert_len++] = ty;
                vertices[vert_len++] = nx;
                vertices[vert_len++] = ny;
                vertices[vert_len++] = nz;

                // Index für das Face speichern (für das EBO)
                face[count++] = vertex_counter++;

                // Nächstes Token auf der Zeile holen
                token = strtok(NULL, " \n");
            } // ENDE der while(token) Schleife

            // Dreiecke aus den gesammelten Face-Vertices bilden (Fan-Triangulation)
            for (int i = 1; i < count - 1; ++i) {
                if (idx_len + 3 > idx_cap) {
                    size_t new_cap = idx_cap == 0 ? 64 : idx_cap * 2;
                    unsigned int *tmp = (unsigned int*)realloc(indices, new_cap * sizeof(unsigned int));
                    if (!tmp) exit(1);
                    indices = tmp;
                    idx_cap = new_cap;
                }

                indices[idx_len++] = face[0];
                indices[idx_len++] = face[i];
                indices[idx_len++] = face[i + 1];
            }
        }    
    }

    fclose(file);
    free(positions);
    free(uvs);
    free(normals);

    Obj obj = {
        .vertices = vertices,
        .vertex_count = vert_len,
        .indices = indices,
        .index_count = idx_len
    };
    return obj;
}

void free_obj(Obj *obj) {
    free(obj->vertices);
    free(obj->indices);
}

#endif

