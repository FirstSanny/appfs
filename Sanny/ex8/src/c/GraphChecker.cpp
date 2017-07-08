/*
 * GraphChecker.cpp
 *
 *  Created on: 08.07.2017
 *      Author: alesan
 */

#include "GraphChecker.h"

GraphChecker::GraphChecker(Edges edges, Nodes nodes) {
	this->edges = edges;
	this->nodes = nodes;
}

GraphChecker::~GraphChecker() {
	// nop
}

char GraphChecker::isConnected(){
	char *visited = new char[nodes.size()];
	for (unsigned int i = 0; i < nodes.size(); i++){
		visited[i] = 0;
	}
	NodesToVisit* nodesToVisit = new NodesToVisit();
	nodesToVisit->push(0);
	visited[0] = 1;

	while(!nodesToVisit->empty()){
		int nodeIndex = nodesToVisit->front();
		nodesToVisit->pop();
		Adjacents* adjs = new Adjacents();
		getAdjacents(nodes[nodeIndex], adjs);
		for(unsigned int i = 0; i < adjs->size(); i++){
			Adjacent adj = adjs->at(i);
			if(visited[adj.first] == 0){
				nodesToVisit->push(adj.first);
				visited[adj.first] = 1;
			}
		}
		delete adjs;

	}
	for (unsigned int i = 0; i < nodes.size(); i++){
		if(visited[i] == 0){
			delete [] visited;
			return 0;
		}
	}
	delete [] visited;
	delete nodesToVisit;
	return 1;
}

char GraphChecker::hasCycle(){
    char* visited = new char[nodes.size()];
    for (unsigned int i = 0; i < nodes.size(); i++){
        visited[i] = 0;
    }
    for(unsigned int i = 0; i < nodes.size(); i++){
    	if(visited[i] == 0){
    		if (hasCycle(i, visited, -1) == 1){
    			delete [] visited;
    			return 1;
    		}
    	}
    }
    delete [] visited;
	return 0;
}

void GraphChecker::getAdjacents(int node, Adjacents* adjs) {
	// collect adjacents
	for (unsigned int i = 0; i < edges.size(); i++) {
		if (edges[i].first != node && edges[i].second != node) {
			continue;
		}
		int endNode = -1;
		if (edges[i].first == node) {
			endNode = edges[i].second;
		} else {
			endNode = edges[i].first;
		}
		for (unsigned int j = 0; j < nodes.size(); j++) {
			if (nodes[j] == endNode) {
				adjs->push_back(std::make_pair(j, endNode));
			}
		}
	}
}

char GraphChecker::hasCycle(int nodeIndex, char* visited, int parentIndex){
	visited[nodeIndex] = 1;
	int node = nodes[nodeIndex];
	Adjacents* adjs = new Adjacents();
	getAdjacents(node, adjs);

    for (unsigned int i = 0; i < adjs->size(); i++){
    	Adjacent adj = adjs->at(i);
        if (!visited[adj.first]){
           if (hasCycle(adj.first, visited, nodeIndex) == 1){
        	   delete adjs;
        	   return 1;
           }
        }

        else if (adj.first != parentIndex){
        	delete adjs;
        	return 1;
        }
    }
    delete adjs;
	return 0;
}

