// Fill out your copyright notice in the Description page of Project Settings.


#include "Pathfinder.h"
//https://en.wikipedia.org/wiki/A*_search_algorithm
struct AStarInternalNode_t{
	int idx;
	TArray<int> edges;
	TArray<double> edge_distances;
	//heuristic function
	double h;
	//cost from the start of the path to the node
	double g;
	double f(){
		return h+g;
	}
	bool operator ==(AStarInternalNode_t other){
		return other.idx == this->idx;
	}
};
static int set_min(TArray<AStarInternalNode_t> set){
	const double inf = 10e15;
	double minval = inf*2;
	double minidx = -1;
	//printf("set size: %zu\n", set.Num());
	for(int i =0; i<set.Num(); i++){
		if(set[i].f()<minval){
			minval = set[i].f();
			minidx =i;
		}
	}
	if(minidx == -1){
		exit(-1);
	}
	return minidx;
}
static TArray<int> reconstruct_path(TArray<int> camefrom, int current){
	TArray<int> out = {};
	while(camefrom[current] != -1){
		out.Add(current);
		current = camefrom[current];
	}
    out.Add(current);
	return out;
}
void SetRemove(TArray<int> &set, int index){
	set.RemoveAt(index);
}
static bool SetContains(TArray<int> set, int value){
	if(set.Num() == 0){
		return false;
	}
	for(int i =0; i<set.Num(); i++){
		if(set[i] == value){
			//printf("%d = %d\n", set[i].idx, value.idx);
			return true;
		}
	}
	//printf("set doesn't contain: %d\n",value.idx);
	return false;
}
static TArray<int> lcl_AStar(TArray<AStarNode_t> in_nodes, int start, int end){
	int swap= start;
	start = end;
	end = swap;
	TArray<AStarInternalNode_t> nodes = {};
	//set up
	const double inf = 10e15;
	for(int i =0; i<in_nodes.Num(); i++){
		AStarInternalNode_t node;
		node.idx = i;
		node.edges = in_nodes[i].edges;
		node.edge_distances = in_nodes[i].distances;
		node.g = inf;
		node.h = in_nodes[i].euc_distance;
		nodes.Add(node);
	}
	nodes[start].g = 0;
	//end set up 
	TArray<int> openset = {start};
	TArray<int> camefrom = {};
	//camefrom set up;
	for(int i =0; i<nodes.Num(); i++){
		camefrom.Add(-1);
	}
	//end camefrom set up
	TArray<double> fscore = {};
	//fscore set up
	for(int i = 0; i<nodes.Num(); i++){
		fscore.Add(inf);
	}
	fscore[start] = nodes[start].h;
	//end fscore set up
	while(openset.Num()>0){
		TArray<AStarInternalNode_t> tmp = {};
		for(int i =0; i<openset.Num(); i++){
			tmp.Add(nodes[openset[i]]);
		}
		int crtmp = set_min(tmp);
		int current = openset[crtmp];
		if(current == end){
			return reconstruct_path(camefrom, current);
		} 
		SetRemove(openset, crtmp);
		for(int i =0; i<nodes[current].edges.Num(); i++){
			int neighbor = nodes[current].edges[i];
			double tmp_g_score = nodes[current].g+nodes[current].edge_distances[i];
			//printf("tmp g score: %f, g score: %f\n", tmp_g_score, nodes[neighbor].g);
			if(tmp_g_score<nodes[neighbor].g){
				camefrom[neighbor] = current;
				nodes[neighbor].g = tmp_g_score;
				if(!SetContains(openset, neighbor)){
					//printf("called\n");
					openset.Add(neighbor);
				}
			}
		}
	}
	//end fscore set up
	return {};
}
TArray<int> UPathfinder::AStar(TArray<AStarNode_t> nodes, int start, int end){
	return lcl_AStar(nodes, start,end);
}


