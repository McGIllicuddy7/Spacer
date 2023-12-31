// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Pathfinder.generated.h"
/**
 * 
 */
struct AStarNode_t{
	//the indices of the edges in the array
	TArray<int> edges;
	//the distances between each edge and the ndoe
	TArray<double> distances;
	//the euclidian distance between the node and the en
	double euc_distance;
};
UCLASS()
class SPACER_API UPathfinder : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// expects a list of AStar_Node_t's each one of these should be initialized with a list of edges and distances, such that the ith distance value shows the distance to the ith distance value, 
	//euc_distance should be initalized with the euclidian distance between the node and the end. index will be initialized to the index of the node in the array, the function returns a list of indices into the array of nodes, 
	//in order of the traversal
	static TArray<int> AStar(TArray<AStarNode_t> nodes, int start, int end);
};
