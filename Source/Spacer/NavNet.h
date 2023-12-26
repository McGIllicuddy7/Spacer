// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavNet.generated.h"
USTRUCT(Blueprintable)
struct FNode_t{
	GENERATED_BODY()
	UPROPERTY()
	//stores the neighbors as indices	
	TArray<int> Neighbors;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FVector Location;
};
UCLASS(Blueprintable)
class SPACER_API ANavNet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANavNet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FNode_t> nodes;
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetNodeLocations();
	UFUNCTION(BlueprintCallable)
	void RegenerateNet();
	UFUNCTION(BlueprintCallable)
	TArray<FVector> Pathfind(FVector Start, FVector End);
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FVector Bounds = {1000,1000,1000};
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Node_Start = {0,0,0};
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Use_Node_Start = false;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float VoxelSize = 200;
private:
	TArray<bool> GenerateVoxelGrid();
	bool BoxTrace(FVector Location, float sidelength); 
	FVector ConvertIndicesToLocation(int x, int y, int z);
	FIntVector ConvertLocationToIndices(FVector Location);
	bool CheckLocationValid(int x, int y, int z);
	int Xex;
	int Yex;
	int Zex;
	bool GridLocationVisibleFromGridLocation(FIntVector location1, FIntVector location2);
	bool RequestNewNode(FVector Location, TArray<bool> &Voxels,TArray<bool> &VisibleLocations);
	int new_visible_from_adding(FIntVector location, TArray<bool> VisibleLocations, TArray<bool> Voxels);
	bool non_visible_locations(TArray<bool> Voxels, TArray<bool> Visible);
	int NearestNode(FVector Location);
public:
	bool CapsuleTrace(FVector Start, FVector End);
	double Infinity();
};
