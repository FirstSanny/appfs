/** @file graph.c
 * @author Franziska Schlösser
 * @date July 2017
 * @brief Short c file containing graph functionality
 */

#include <limits.h>
#include "graph.h"

void update_neighbor_info(
        GraphSearch *gs,
        unsigned int curr) {

    for (int i = 0; i < gs->g->n_neighbors[curr]; i++) {
        unsigned int n = gs->g->neighbors[curr][2*i] - 1; // n is index of neighbor vertex
        unsigned int w = gs->g->neighbors[curr][(2*i)+1]; // w is weight of edge from curr to n
        if (!gs->visited[n]) {
            unsigned int newdist = gs->distances[curr] + w;
            unsigned int olddist = gs->distances[n];
            if (olddist == -1 || newdist < olddist) {
                gs->distances[n] = newdist;
                gs->prev[n] = curr;
            }
            bool found = false;
            for (int k = 0; k < gs->n_to_visit; k++) {
                if (gs->to_visit[k] == n) {
                    found = true;
                }
            }
            if (!found) {
                gs->to_visit[gs->n_to_visit] = n;
                gs->n_to_visit = gs->n_to_visit+1;
                assert(gs->n_to_visit <= gs->g->n_verts);
            }
        }
    }
}

void run_dijkstra(
        GraphSearch *gs) {

    while (gs->n_to_visit != 0) { 
        unsigned int curr; // curr is the index of the current vertex
        curr = get_next_destination(gs->to_visit, gs->n_to_visit, gs->distances); 
        gs->n_to_visit--;
        gs->visited[curr] = true;
        update_neighbor_info(gs, curr);
    }
}

void add_closest_terminal(
        Graph *g, 
        unsigned int *vertex_mask,
        unsigned int *prev) {
    // at i vertex_mask is 0 or 1 on vertices not contained in subgraph, 2 or 3 on vertices that are contained

    unsigned long *distances = malloc(sizeof(*distances) * g->n_verts);
    unsigned int *to_visit = malloc(sizeof(*to_visit) * g->n_verts); // holds indices of vertices in queue to visit
    bool *visited = malloc(sizeof(*visited) * g->n_verts);
    // copy vertex_mask to mask (all visited and dist 0)
    for (int i = 0; i < g->n_verts; i++) {
        if (vertex_mask[i] > 1) {
            // distances on subgraph are 0
            distances[i] = 0;
            // on subgraph everything is visited
            visited[i] = true;
        } else {
            // distances are inf on complement of subgraph
            distances[i] = UINT_MAX;
            // on complement of subgraph everything is unvisited
            visited[i] = false;
        }
        // init to_visit. nothing on this list yet
        to_visit[i] = UINT_MAX;
    }

    GraphSearch *gs = malloc(sizeof(*gs));
    gs->g = g;
    gs->distances = distances;
    gs->visited = visited;
    gs->to_visit = to_visit;
    gs->n_to_visit = 0;
    gs->prev = prev;

    for (int i = 0; i < g->n_verts; i++) {
        if (vertex_mask[i] > 1) {
            update_neighbor_info(gs, i);
        }
    }
    
    run_dijkstra(gs);

    join_closest_terminal(distances, g->n_verts, vertex_mask, prev);

    free(distances);
    free(to_visit);
    free(visited);
    free(gs);
}

bool unconnected_terminals(
        Graph *g, 
        unsigned int *vertex_mask) {
    for (int i = 0; i < g->n_verts; i++) {
        if (vertex_mask[i] == 1) {
            return true;
        } 
    }
    return false;
}

unsigned int* steiner(
        Graph *g, 
        unsigned int *vertex_mask) {
    unsigned int count = 0;
    unsigned int found = UINT_MAX;
    for (int i = 0; i < g->n_verts; i++) {
        if (vertex_mask[i] == 1) {
            count = count+1;
            if (found == UINT_MAX) {
                vertex_mask[i] = 3;
                found = i;
            }
        }
        if(count > 1) {
            break;
        }
    }
    assert(count>1); // want at least two terminals
    assert(found >= 0 && found < g->n_verts);

    unsigned int *prev = malloc(sizeof(*prev) * g->n_verts); // holds indices of predecessor in steiner tree
    for (int i = 0; i < g->n_verts; i++) {
        prev[i] = UINT_MAX;
    }

    while (unconnected_terminals(g, vertex_mask)) { 
        add_closest_terminal(g, vertex_mask, prev);
    }
    return prev;
}

void free_graph(
        Graph *g) {
    
    for (int i = 0; i < g->n_verts; i++) {
        free(g->neighbors[i]);
    }
    free(g->neighbors);
    free(g->n_neighbors);
}

unsigned int get_next_destination(
        unsigned int *to_visit, 
        unsigned int n_to_visit, 
        unsigned long *distances) {
    
    unsigned int mind = UINT_MAX;
    unsigned int minv = 0;
    unsigned int ind = 0;
    for (int i = 0; i < n_to_visit; i++) {
        unsigned int v = to_visit[i];
        unsigned long d = distances[v];
        if (d < mind) {
            mind = d;
            minv = v;
            ind = i;
        }
    }
    to_visit[ind] = to_visit[n_to_visit-1];
    return minv;
}

void read_numbers(
        unsigned int *res, 
        int n, 
        char *line) {

    char* s = &line[0];
    char e[LEN_NUMBER];
    memset(e, 0, sizeof(e));
    for (int i = 0; i < n; i++) {
        int count = 0;
        while(isdigit(*s)) {
            e[count] = *s;
            count++;
            s++;
        }
        e[count] = '\0';
        s++;
        res[i] = atoi(e);
    }
}

unsigned int read_graph_file(
        Graph *g, 
        char *file, 
        unsigned int ***edges) {

    FILE *f = fopen(file, "r");
    if (NULL == f) {
        printf("File does not exist. Aborting.\n");
        exit(0);
    }

    // assume vertices are numbered 1 .. n
    // assume each edge is only described once
    // assume graph is undirected

    // assume line never longer than ...
    char line[3*LEN_NUMBER]; 

    // assume first line holds number of vertices and number of edges
    fgets(line, sizeof(line), f);

    unsigned int cardinalities[2]; // store number of vertices, number of edges
    read_numbers(cardinalities, 2, line);
    g->n_verts = cardinalities[0];
    unsigned int n_edges = cardinalities[1];

    // alloc memory according to given sizes
    g->n_neighbors = malloc(sizeof(*(g->n_neighbors)) * g->n_verts);
    memset(g->n_neighbors, 0, sizeof(*(g->n_neighbors)) * g->n_verts);

    *edges = malloc(sizeof(**edges) * n_edges);
    for(int i = 0; i < n_edges; i++) {
        (*edges)[i] = malloc(sizeof(*((*edges)[i])) * 3);
        memset((*edges)[i], 0, sizeof(*((*edges)[i])) * 3);
    }

    // read file line by line
    // count number of neighbors for each vertex
    unsigned int count = 0;
    while (NULL != fgets(line, sizeof(line), f)) {
        // numbers contains source_vertex, target_vertex and weight of edge
        unsigned int *numbers = (*edges)[count];
        read_numbers(numbers, 3, line);
        g->n_neighbors[numbers[0]-1]++;
        g->n_neighbors[numbers[1]-1]++;
        count++;
    }
    fclose(f);
    return n_edges;
}

void fill_neighbors(
        Graph *g, 
        unsigned int** edges, 
        unsigned int n_edges) {

    g->neighbors = malloc(sizeof(*(g->neighbors)) * g->n_verts);
    for(int i = 0; i < g->n_verts; i++) {
        g->neighbors[i] = malloc(sizeof(*(g->neighbors[i])) * g->n_neighbors[i] * 2);
        memset(g->neighbors[i], 0, sizeof(*(g->neighbors[i])) * g->n_neighbors[i] * 2);
    }
    memset(g->n_neighbors, 0, sizeof(*g->n_neighbors) * g->n_verts);
    for (int i = 0; i < n_edges; i++) {
        unsigned int v[2];
        memset(v, 0, sizeof(*v) * 2);
        v[0] = edges[i][0];
        v[1] = edges[i][1];
        unsigned int weight = edges[i][2];
        for (int j = 0; j < 2; j++) {
            unsigned int pos = g->n_neighbors[v[j]-1]*2;
            g->neighbors[v[j]-1][pos] = v[(j+1)%2];
            g->neighbors[v[j]-1][pos+1] = weight;
            g->n_neighbors[v[j]-1]++;
        }
    }
    for(int i = 0; i < n_edges; i++) {
        free(edges[i]);
    }
}

unsigned long* shortest_distances_to(
        Graph *g, 
        unsigned int destination,
        unsigned int *prev) {

    unsigned long *distances = malloc(sizeof(*distances) * g->n_verts);
    unsigned int *to_visit = malloc(sizeof(*to_visit) * g->n_verts); // holds indices of vertices in queue to visit
    for (int i = 0; i < g->n_verts; i++) {
        distances[i] = UINT_MAX;
        to_visit[i] = UINT_MAX;
        prev[i] = UINT_MAX;
    }
    bool *visited = malloc(sizeof(*visited) * g->n_verts);
    memset(visited, 0, sizeof(*visited) * g->n_verts);

    distances[destination-1] = 0;
    to_visit[0] = destination-1;

    GraphSearch *gs = malloc(sizeof(*gs));
    gs->g = g;
    gs->distances = distances;
    gs->visited = visited;
    gs->to_visit = to_visit;
    gs->n_to_visit = 1;
    gs->prev = prev;

    run_dijkstra(gs);
    
    free(to_visit);
    free(visited);
    free(gs);
    return distances;
}

unsigned long* find_longest(
        unsigned long *distances, 
        int n_verts) {

    unsigned long maxd = 0;
    unsigned int maxv = 0; // index of vert
    for (int i = 0; i < n_verts; i++) {
        unsigned long dist = distances[i];
        if (dist > maxd) {
            maxd = dist;
            maxv = i;
        }
    }
    unsigned long* res = malloc(sizeof(*res) * 2);
    res[0] = maxd;
    res[1] = maxv;
    return res;
}

void join_closest_terminal(
        unsigned long *distances, 
        int n_verts,
        unsigned int *vertex_mask,
        unsigned int *prev) {

    unsigned long mind = ULONG_MAX;
    unsigned int minv = UINT_MAX; // index of vert
    for (int i = 0; i < n_verts; i++) {
        unsigned long dist = distances[i];
        if (dist < mind && dist != 0 && vertex_mask[i]==1) {
            mind = dist;
            minv = i;
        }
    }
    printf("Joining terminal %d to subgraph via path: ", minv+1);
    unsigned int walker = minv;
    // minv holds closest neighboring terminal
    while (vertex_mask[walker] < 2) {
        printf("%d ", walker+1);
        vertex_mask[walker] = vertex_mask[walker]+2;
        walker = prev[walker];
    }
    printf("\n");
}

unsigned long weight_of_tree(
        Graph *g,
        unsigned int *vertex_mask,
        unsigned int *prev) {
     
    unsigned long treeweight = 0;
    for(int i = 0; i < g->n_verts; i++) {
        if(vertex_mask[i] > 1 && prev[i] != UINT_MAX) { // for all terminals that are not root
            treeweight = treeweight + edgeweight(g, i, prev[i], false);
        }
    }
    return treeweight;
}

unsigned long edgeweight(
        Graph *g,
        unsigned int v1,
        unsigned int v2, 
        bool directed) {

    if (true == directed) {
        for (int i = 0; i < g->n_neighbors[v1]; i++) {
            if (g->neighbors[v1][2*i] == v2 + 1) {
                return g->neighbors[v1][(2*i)+1];
            }
        }
        return ULONG_MAX;
    } else {
        unsigned long w1 = edgeweight(g, v1, v2, true);
        unsigned long w2 = edgeweight(g, v2, v1, true);
        unsigned long weight = (w1 < w2 ? w1 : w2);
        assert(weight < ULONG_MAX);
        return weight;
    }
}
