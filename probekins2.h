#ifndef PROBEKINS2_H
#define PROBEKINS2_H

#define SHMEM_KEY 4715 // a famous and very old-fashioned german perfume brand


#pragma pack(push, 4)
typedef struct {
    double coord[3];
    int face_group_size;
} vertex_t;

typedef vertex_t* vertex_ptr;

typedef int face_t[3];
typedef vertex_ptr face2_t[3];
typedef face2_t* face2_ptr;

typedef struct {
    int n_vertices;
    int n_faces;
    int n_face_groups;
    vertex_t vertices[1]; //...n_vertices
    // followed by:
    // face_t faces[1]; // 1..n fac
    // starting at &vertices[n_vertices]
    // followed by:
    // int face_groups[1]; // 1..n_face_groups
    // starting at &faces[n_faces]
} mesh_struct;
#pragma pack(pop)

#endif //PROBEKINS_H
