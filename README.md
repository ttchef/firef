
# firef

A minimal Obj loader in C
The name comes from the popular frief!

---
main.c contains a example of how to use this libary

# Example 
```
#include <stdio.h> 

#define FIREF_IMPL
#include "firef.h"


int main() {
    const char *path = "cube.obj";
    Obj mesh = load_obj(path);
    printf("Loaded %zu floats (%zu vertices), %zu indices from %s\n",
           mesh.vertex_count, mesh.vertex_count / 8, mesh.index_count, path);

    printf("\n==== Vertex Dump ====\n");
    for (size_t i = 0; i < mesh.vertex_count; i += 8) {
        printf("Vertex %zu:\n", i / 8);
        printf("  Position:  %.3f, %.3f, %.3f\n", mesh.vertices[i], mesh.vertices[i + 1], mesh.vertices[i + 2]);
        printf("  UV:        %.3f, %.3f\n", mesh.vertices[i + 3], mesh.vertices[i + 4]);
        printf("  Normal:    %.3f, %.3f, %.3f\n", mesh.vertices[i + 5], mesh.vertices[i + 6], mesh.vertices[i + 7]);
    }

    printf("\n==== Index Dump ====\n");
    for (size_t i = 0; i < mesh.index_count; i += 3) {
        printf("Triangle %zu: %u, %u, %u\n", i / 3, mesh.indices[i], mesh.indices[i + 1], mesh.indices[i + 2]);
    }

    free_obj(&mesh);
    return 0;
}

```
