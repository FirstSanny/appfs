/**
 * @file main.c
 * @author Franziska Schlösser
 * @date July 2017
 * @brief Main programm fulfilling ex8
 */

#include <time.h>
#include <math.h>
#include "graph.h"

/** @brief Returns a list of primes
 *
 * @param upper_bound biggest potential prime to be returned
 * @return sieve number of primes found
 */
signed int* get_primes(signed int upper_bound) {
    assert(upper_bound > 1);
    signed int *sieve = malloc(sizeof(*sieve) * (upper_bound+1));
    for(int i = 0; i <= upper_bound; i++) {
        sieve[i] = 1;
    }
    sieve[0] = 0; // 1 is not prime
    signed int bound = sqrt(upper_bound);
    for(int i = 2; i <= bound; i++) {
        if(sieve[i-1]==1) { // if i is prime
            // multiples of i are not prime
            signed int j = 2*i;
            while(j <= upper_bound) {
                sieve[j-1] = 0;
                j += i;
            }
        }
    }
    return sieve;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Please provide filename containing as first and only argument.");
        exit(0);
    }

// ##### init from file
    // filename is first commandlinearg
    char *file = argv[1];
    // destination is source node
    signed int destination = 1;

    Graph *g = malloc(sizeof(Graph));

    signed int **edges;
    signed int n_edges = read_graph_file(g, file, &edges);

    //float density = ((float)n_edges)/(n_verts/2 * n_verts);
    //bool sparse = (density < 0.1 ? true : false);

    // sort edges to neighbors
    fill_neighbors(g, edges, n_edges); // for each vertex this holds a list of neighbors with weights
    free(edges);

// ##### find lengths of shortest paths to destination
    // measure time
    clock_t start = clock();
    signed int *vertex_mask = get_primes(g->n_verts);
    // steiner tree connecting prime nodes
    steiner(g, vertex_mask);//TODO
    clock_t end = clock();
    float seconds = (float)(end - start) / CLOCKS_PER_SEC;

// ##### free memory of graph
    free_graph(g);
    free(g);

    free(terminals);
    return 0; // everything went fine (hopefully)
}

