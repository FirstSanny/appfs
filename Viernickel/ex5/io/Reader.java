package io;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import datastructure.*;

/**
 * Reads a file of the format:
 * nVertices nEdges
 * headId tailId weight
 * headId tailId weight
 * .
 * .
 * .
 * 
 * Saves nodes in an array and edges as neighbours within the Node datastructure
 * @author merlin
 * @date June 08 2017
 */
public class Reader{
    
    /**
     * Reads the file returns a Node array
     * @param path Path to file
     * @return Array of nodes read
     */
    public static Node[] readFile(String path){
        FileReader fr = null;
        BufferedReader br;
        String line;
        String[] split;
        int nNodes;
        int nEdges;
        Node[] nodes = null;
        int headId;
        int tailId;
        int weight;
        int i;
        int j;
        
        
        try {
            fr = new FileReader(path);
            br = new BufferedReader(fr);
            
            /** Read first line
             */
            line = br.readLine();
            nNodes = Integer.valueOf(line.split(" ")[0]);
            nEdges = Integer.valueOf(line.split(" ")[1]);
            
            /** Initialize node array
             */
            nodes = new Node[nNodes];
            for(i=0; i<nNodes; i++){
                nodes[i] = new Node(i);
            }
            nodes[0].distance = 0;
            
            /** Read and save edges
             */
            for(j=0; j<nEdges; j++){
                line = br.readLine();
                split = line.split(" ");
                headId = Integer.valueOf(split[0])-1;
                tailId = Integer.valueOf(split[1])-1;
                weight = Integer.valueOf(split[2]);
                
                nodes[headId].neighbours.add(new Neighbour(tailId, weight));
                nodes[tailId].neighbours.add(new Neighbour(headId, weight));
            }
            
            br.close();
            } catch (IOException e) {
            e.printStackTrace();
        }
        return nodes;
    }
}