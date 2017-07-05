/**
 * @file main.c
 * @author Franziska Schlösser
 * @date July 2017
 * @brief Main programm fulfilling ex5, 6 and 7
 */

#include <time.h>
#include "graph.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Please provide filename containing as first and only argument.");
        exit(0);
    }

// ##### init from file

    // filename is first commandlinearg
    char *file = argv[1];
    // destination is source node
    unsigned int destination = 1;

    Graph *g = malloc(sizeof(Graph));

    unsigned int **edges;
    unsigned int n_edges = read_graph_file(g, file, &edges);

    //float density = ((float)n_edges)/(n_verts/2 * n_verts);
    //bool sparse = (density < 0.1 ? true : false);

    // sort edges to neighbors
    fill_neighbors(g, edges, n_edges); // for each vertex this holds a list of neighbors with weights
    free(edges);

// ##### find lengths of shortest paths to destination
    // measure time
    unsigned int *prev = malloc(sizeof(*prev) * g->n_verts);
    clock_t start = clock();
    unsigned long *distances = shortest_distances_to(g, destination, prev);
    clock_t end = clock();
    float seconds = (float)(end - start) / CLOCKS_PER_SEC;

    for (int i = 0; i < g->n_verts; i++) {
        printf("predecessor of %d is %d\n", i+1, prev[i]+1);
    }

// ##### free memory of graph
    free_graph(g);

// ##### find longest amongst shortests
    unsigned long *res = find_longest(distances, g->n_verts);
    free(g);
    free(prev);
    free(distances);

    printf("RESULT VERTEX %li\nRESULT DIST %li\nRESULT TIME %.12f\n", res[1]+1, res[0], seconds);
    free(res);
    return 0; // everything went fine (hopefully)
}

