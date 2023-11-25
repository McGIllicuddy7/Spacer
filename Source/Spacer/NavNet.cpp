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
	RegenerateNet();
	
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
	Xex = ceil(Bounds.X/VoxelSize);
	Yex = ceil(Bounds.Y/VoxelSize);
	Zex = ceil(Bounds.Z/VoxelSize);
	for(int z = 0; z<Zex; z++){
		for(int y =0; y<Yex; y++){
			for(int x =0; x<Xex; x++){
				FVector loc = {(float)x,(float)y,(float)z};
				loc *= VoxelSize;
				loc+= GetActorLocation();
				out.Add(BoxTrace(loc, VoxelSize));
			}
		}
	}
	return out;

}
TArray<FVector> ANavNet::GetNodeLocations(){
		return {};
}
void ANavNet::RecalculateNodeVisibilities(TArray<bool> &Voxels, TArray<bool> &VisibleLocations){

}
void ANavNet::RegenerateNet(){
	TArray<bool> Voxels = GenerateVoxelGrid();
	TArray<bool> VisibleLocations;
	for(int i = 0; i<Voxels.Num(); i++){
		VisibleLocations.Add(false);
	}

}
TArray<FVector> ANavNet::Pathfind(FVector Start, FVector End){
	return {};
}
