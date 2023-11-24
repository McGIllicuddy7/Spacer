// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavNet.generated.h"
struct Node_t{
	Node_t * Neighbors[16];
	int NumNeighbors;
	FVector Location;
};
UCLASS()
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
	TArray<Node_t> nodes;
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetNodeLocations();
	UFUNCTION(BlueprintCallable)
	void RegenerateNet();
	UFUNCTION(BlueprintCallable)
	TArray<FVector> Pathfind(FVector Start, FVector End);
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FVector Bounds = {1000,1000,1000};
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float VoxelSize = 100;
private:
	TArray<bool> GenerateVoxelGrid();
	bool BoxTrace(FVector Location, float sidelength);
};
