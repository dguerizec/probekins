#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_DIM 3
typedef double vertex_t[MAX_DIM];
typedef vertex_t* face_t[3];
typedef vertex_t* vertex_ptr;

struct kd_node_t{
    //double v[MAX_DIM];
    vertex_t* v;
    face_t** faces; // list of facets which node does include
    struct kd_node_t *left, *right;
};

double
dist(struct kd_node_t *a, struct kd_node_t *b, int dim)
{
    double t, d = 0;
    while (dim--)
    {
        vertex_ptr a_ptr = a->v;
        vertex_ptr b_ptr = b->v;

        t = (*a_ptr)[dim] - (*b_ptr)[dim];
        d += t * t;
    }
    return d;
}

void swap(struct kd_node_t *a_node, struct kd_node_t *b_node)
{
    struct kd_node_t tmp_node;

    memcpy(&tmp_node,  a_node, sizeof(struct kd_node_t));
    memcpy(a_node,     b_node, sizeof(struct kd_node_t));
    memcpy(b_node,  &tmp_node, sizeof(struct kd_node_t));
}

/* see quickselect method */
struct kd_node_t*
find_median(struct kd_node_t *start, struct kd_node_t *end, int idx)
{
    if (end <= start) return NULL;
    if (end == start + 1)
        return start;

    struct kd_node_t *p, *store, *md = start + (end - start) / 2;
    double pivot;
    while (1) {
        vertex_ptr v_ptr = md->v;
        pivot = (*v_ptr)[idx];

        swap(md, end - 1);
        for (store = p = start; p < end; p++)
        {
            vertex_ptr v_ptr = p->v;
            if ((*v_ptr)[idx] < pivot) {
                if (p != store)
                    swap(p, store);
                store++;
            }
        }
        swap(store, end - 1);

        /* median has duplicate values */
        v_ptr = md->v;
        vertex_ptr store_v_ptr = store->v;
        if ((*store_v_ptr)[idx] == (*v_ptr)[idx])
            return md;

        if (store > md) end = store;
        else        start = store;
    }
}

struct kd_node_t*
make_tree(struct kd_node_t *t, int len, int i, int dim)
{
    struct kd_node_t *n;

    if (!len) return 0;

    if ((n = find_median(t, t + len, i))) {
        i = (i + 1) % dim;
        n->left  = make_tree(t, n - t, i, dim);
        n->right = make_tree(n + 1, t + len - (n + 1), i, dim);
    }
    return n;
}

/* global variable, so sue me */
int visited;

void nearest(struct kd_node_t *root, struct kd_node_t *nd, int i, int dim,
        struct kd_node_t **best, double *best_dist)
{
    double d, dx, dx2;

    if (!root) return;

    d = dist(root, nd, dim);

    vertex_ptr root_v_ptr = root->v;
    vertex_ptr nd_v_ptr = nd->v;

    dx = (*root_v_ptr)[i] - (*nd_v_ptr)[i];
    dx2 = dx * dx;

    visited++;

    if (!*best || d < *best_dist) {
        *best_dist = d;
        *best = root;
    }

    /* if chance of exact match is high */
    if (!*best_dist) return;

    if (++i >= dim) i = 0;

    nearest(dx > 0 ? root->left : root->right, nd, i, dim, best, best_dist);
    if (dx2 >= *best_dist) return;
    nearest(dx > 0 ? root->right : root->left, nd, i, dim, best, best_dist);
}

int main(void)
{
    // Array of vertices
    vertex_t v[] = {
        {-150.0,  111.0, 2.1}, // 0
        { -12.0,  180.0, 1.4}, // 1
        { -44.0, -121.0, 1.2}, // 2
        { 210.0, -120.0, 0.2}, // 3
        {-100.0,  200.0, 0.5}, // 4
    };

    // Array of faces
    face_t f[] = {
        {&v[0], &v[1], &v[2]},
        {&v[1], &v[3], &v[2]},
        {&v[4], &v[1], &v[0]},
        NULL
    };

    face_t* faces0[] = {&f[0], &f[1], NULL};
    face_t* faces1[] = {&f[0], &f[1], &f[2], NULL};
    face_t* faces2[] = {&f[0], &f[1], NULL};
    face_t* faces3[] = {&f[1], NULL};
    face_t* faces4[] = {&f[2], NULL};

    struct kd_node_t nodes[] = {
        {
            &v[0],
            faces0
        },
        {
            &v[1],
            faces1
        },
        {
            &v[2],
            faces2
        },
        {
            &v[3],
            faces3
        },
        {
            &v[4],
            faces4
        }
    };

    vertex_t testVertex = {200.0, -200.0, 0.0};

    struct kd_node_t testNode = { .v = &testVertex, NULL, NULL, NULL };
    struct kd_node_t *root, *found, *million;
    double best_dist;

    //printf(">> WP tree\nsearching for v0(%g), v1(%g)\n", **(testNode.v), *(*(testNode.v)+1));
    vertex_ptr target_vertex = testNode.v;
    printf(">> Searching for vertex (x=%g, y=%g, z=%g) in kd-tree\n",
            (*target_vertex)[0], (*target_vertex)[1], (*target_vertex)[2]);
    int nodes_number = sizeof(nodes) / sizeof(struct kd_node_t);
    printf("Number of nodes = %d\n", nodes_number);

    root = make_tree(nodes, nodes_number, 0, 2);

    nearest(root, &testNode, 0, 2, &found, &best_dist);

    vertex_ptr found_v_ptr = found->v;
    printf("found nearest vertex (x=%g, y=%g, z=%g) dist %g\nseen %d nodes\n\n",
            (*found_v_ptr)[0], (*found_v_ptr)[1], (*found_v_ptr)[2],
            sqrt(best_dist), visited);
}

#if 0
#define N 1000000
#define rand1() (rand() / (double)RAND_MAX)
#define rand_pt(v) { v.x[0] = rand1(); v.x[1] = rand1(); v.x[2] = rand1(); }
int main1(void)
{
    int i;
    struct kd_node_t wp[] = {
        {},
        {{2, 3}}, {{5, 4}}, {{9, 6}}, {{4, 7}}, {{8, 1}}, {{7, 2}}
    };
    struct kd_node_t testNode = {{9, 2}};
    struct kd_node_t *root, *found, *million;
    double best_dist;

    root = make_tree(wp, sizeof(wp) / sizeof(wp[1]), 0, 2);

    visited = 0;
    found = 0;
    nearest(root, &testNode, 0, 2, &found, &best_dist);

    printf(">> WP tree\nsearching for (%g, %g)\n"
            "found (%g, %g) dist %g\nseen %d nodes\n\n",
            testNode.x[0], testNode.x[1],
            found->v[0], found->v[1], sqrt(best_dist), visited);

    million =(struct kd_node_t*) calloc(N, sizeof(struct kd_node_t));
    srand(time(0));
    for (i = 0; i < N; i++) rand_pt(million[i]);

    root = make_tree(million, N, 0, 3);
    rand_pt(testNode);

    visited = 0;
    found = 0;
    nearest(root, &testNode, 0, 3, &found, &best_dist);

    printf(">> Million tree\nsearching for (%g, %g, %g)\n"
            "found (%g, %g, %g) dist %g\nseen %d nodes\n",
            testNode.x[0], testNode.x[1], testNode.x[2],
            found->v[0], found->v[1], found->v[2],
            sqrt(best_dist), visited);

    /* search many random points in million tree to see average behavior.
       tree size vs avg nodes visited:
       10      ~  7
       100     ~ 16.5
       1000        ~ 25.5
       10000       ~ 32.8
       100000      ~ 38.3
       1000000     ~ 42.6
       10000000    ~ 46.7              */
    int sum = 0, test_runs = 100000;
    for (i = 0; i < test_runs; i++) {
        found = 0;
        visited = 0;
        rand_pt(testNode);
        nearest(root, &testNode, 0, 3, &found, &best_dist);
        sum += visited;
    }
    printf("\n>> Million tree\n"
            "visited %d nodes for %d random findings (%f per lookup)\n",
            sum, test_runs, sum/(double)test_runs);

    // free(million);

    return 0;
}
#endif
