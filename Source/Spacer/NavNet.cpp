// Fill out your copyright notice in the Description page of Project Settings.


#include "NavNet.h"
#include "Kismet/KismetSystemLibrary.h"
// Sets default values
ANavNet::ANavNet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	//RegenerateNet();
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
	return UKismetSystemLibrary::BoxTraceSingle(
		GetWorld(), 
		Location,
		Location,
		(FVector){sidelength/2,sidelength/2,sidelength/2}, 
		(FRotator){90,0,0}, 
		ETraceTypeQuery::TraceTypeQuery1,
		false, 
		{this},
		EDrawDebugTrace::None,
		res,
		false);
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
		TArray<FVector> out;
		for(int i =0; i<num_nodes; i++){
			out.Add(nodes[i].Location);
		}
		return out;
}
bool ANavNet::RequestNewNode(Node_t * parent, FVector Location, TArray<bool> &Voxels, TArray<bool> &VisibleLocations, int recursion_depth){
	if(num_nodes>64){
		return false;
	}
	Node_t n;
	n.Location = Location;
	n.NumNeighbors = 0;
	for(int i = 0; i<16; i++){
		n.Neighbors[i] = 0;
	}
	nodes[num_nodes] = n;
	num_nodes ++;
	FIntVector loc;
	FVector loctmp = (Location-GetActorLocation())/VoxelSize;
	loc.X = loctmp.X;
	loc.Y = loctmp.Y;
	loc.Z = loctmp.Z;
	for(int z = 0; z<Zex; z++){
		for(int y = 0; y<Yex; y++){
			for(int x =0; x<Xex; x++){
				if(!Voxels[z*Yex*Xex+y*Xex+x]){
					if(!GridLocationVisibleFromGridLocation(loc, {x,y,z})){
						VisibleLocations[z*Yex*Xex+y*Xex+x] = true;
					}
				}
			}
		}
	}
	return true;
}
int ANavNet::new_visible_from_adding(FIntVector location, TArray<bool> VisibleLocations, TArray<bool> Voxels){
	int out = 0;
	for(int z = 0; z<Zex; z++){
		for(int y = 0; y<Yex; y++){
			for(int x =0; x<Xex; x++){
				if(!VisibleLocations[z*Yex*Xex+y*Xex+x] && !Voxels[z*Yex*Xex+y*Xex+x]){
					FIntVector loc = {x,y,z};
					if(!GridLocationVisibleFromGridLocation(location, loc)){
						out++;
					}
				}
			}
		}
	}
	return out;
}
bool ANavNet::non_visible_locations(TArray<bool> Voxels, TArray<bool> Visible){
	for(int i =0; i<Voxels.Num(); i++){
		if(Voxels[i] == false && Visible[i] == false){
			return true;
		}
	}
	return false;
}
void ANavNet::RegenerateNet(){
	num_nodes = 0;
	TArray<bool> Voxels = GenerateVoxelGrid();
	TArray<bool> VisibleLocations;
	for(int i = 0; i<Voxels.Num(); i++){
		VisibleLocations.Add(false);
	}
	memset(nodes, 0, 1024*sizeof(Node_t));
	if(!Use_Node_Start){
		Node_Start = GetActorLocation();
	}
	RequestNewNode(NULL,Node_Start, Voxels, VisibleLocations, 0);
	while(true){
		FIntVector MaxLocation = {-1,-1,-1};
		int MaxVisible = -1;
		for(int z = 0; z<Zex; z++){
			for(int y =0; y<Yex; y++){
				for(int x =0; x<Xex; x++){
					if(VisibleLocations[z*Yex*Xex+y*Yex+x]){
						int max = new_visible_from_adding({x,y,z}, VisibleLocations, Voxels);
						if(max>MaxVisible){
							MaxLocation = {x,y,z};
							MaxVisible = max;
						}
					}
				}
			}
		}
		if(MaxVisible == 0){
			break;
		}
		FVector loc = {(float)MaxLocation.X, (float)MaxLocation.Y, (float)MaxLocation.Z};
		loc*= VoxelSize;
		loc+=GetActorLocation();
		bool s = RequestNewNode(NULL,loc, Voxels, VisibleLocations, 0);
		if(!s){
			break;
		}
	}
}
TArray<FVector> ANavNet::Pathfind(FVector Start, FVector End){
	return {};
}
FVector ANavNet::ConvertIndicesToLocation(int x, int y, int z){
	FVector loc = {(float)x,(float)y, (float)z};
	loc.X *= VoxelSize;
	loc.Y *= VoxelSize;
	loc.Z *= VoxelSize;
	return loc + GetActorLocation();
}
FIntVector ANavNet::ConvertLocationToIndices(FVector Location){
	FVector locn = Location-GetActorLocation();
	locn.X /= Bounds.X;
	locn.Y /= Bounds.Y;
	locn.Z /= Bounds.Z;
	return {(int)floor(locn.X), (int)floor(locn.Y), (int)floor(locn.Z)};
}
bool ANavNet::CheckLocationValid(int x, int y, int z){
	bool xv = x>-1 && x<Xex;
	bool yv = y>-1 && y<Yex;
	bool zv = z>-1 && z>Zex;
	return xv&&yv&&zv;
}
bool ANavNet::GridLocationVisibleFromGridLocation(FIntVector loc1, FIntVector loc2){
	if(loc1 == loc2){
		return false;
	}
	FHitResult hit;
	TArray<AActor*> ignored = {};
	return UKismetSystemLibrary::CapsuleTraceSingle(
		GetWorld(),
		ConvertIndicesToLocation(loc1.X, loc1.Y, loc1.Z), 
		ConvertIndicesToLocation(loc2.X, loc2.Y, loc2.Z),
		30,
		30,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ignored,
		EDrawDebugTrace::None,
		hit,
		true,
		(FLinearColor){1,0,0,0},
		(FLinearColor){0,1,0,9},
		128
	);	
}