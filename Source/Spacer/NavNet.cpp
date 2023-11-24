// Fill out your copyright notice in the Description page of Project Settings.


#include "NavNet.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ANavNet::ANavNet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ANavNet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANavNet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
bool ANavNet::BoxTrace(FVector Location, float sidelength){
	FHitResult res;
	return UKismetSystemLibrary::BoxTraceSingle(GetWorld(), Location,Location,(FVector){sidelength/2,sidelength/2,sidelength/2}, (FRotator){90,0,0}, ETraceTypeQuery::TraceTypeQuery1,false, {this},EDrawDebugTrace::None,res,false);
}
TArray<bool> ANavNet::GenerateVoxelGrid(){
	TArray<bool> out;
	return out;

}
TArray<FVector> ANavNet::GetNodeLocations(){
		return {};
}
void ANavNet::RegenerateNet(){
	TArray<bool> Voxels = GenerateVoxelGrid();
}
TArray<FVector> ANavNet::Pathfind(FVector Start, FVector End){
	return {};
}
