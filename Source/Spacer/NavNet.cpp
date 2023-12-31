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
		for(int i =0; i<nodes.Num(); i++){
			out.Add(nodes[i].Location);
		}
		return out;
}
bool ANavNet::RequestNewNode(FVector Location, TArray<bool> &Voxels, TArray<bool> &VisibleLocations){
	FNode_t n;
	n.Location = Location;
	n.Neighbors.Empty();
	nodes.Add(n);
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
	TArray<bool> Voxels = GenerateVoxelGrid();
	TArray<int> Table = {};
	for(int z =0; z<Zex; z++){
		for(int y = 0; y<Yex;y++){
			for(int x =0; x<Xex; x++){
				int count = z*Yex*Xex+y*Xex+x;
				if(!Voxels[count]){
					FNode_t node;
					node.Neighbors = {};
					node.Location = ConvertIndicesToLocation(x,y,z);
					Table.Add(nodes.Num());
					nodes.Add(node);
				}
				else{
					Table.Add(-1);
				}
			}
		}
	}
	for(int i =0; i<nodes.Num(); i++){
		for(int z =-1; z<2; z++){
			for(int y =-1; y<2; y++){	
				for(int x =-1; x<2; x++){
					if(z == 0 && y == 0 && x == 0){
						continue;
					}
					FIntVector indices = ConvertLocationToIndices(nodes[i].Location);
					if((indices.X == 0 && x == -1 )||( indices.X >= Xex-1 && x == 1)){
						continue;
					}
					if((indices.Y == 0 && y == -1 )|| (indices.Y >= Yex-1 && y == 1)){
						continue;
					}
					if((indices.Z == 0 && z == -1 )|| (indices.Z >= Zex-1 && z == 1)){
						continue;
					}
					FIntVector v = ConvertLocationToIndices(nodes[i].Location);
					if(Voxels[(z+v.Z)*Yex*Xex+(y+v.Y)*Xex+(x+v.X)] && Table[(z+v.Z)*Yex*Xex+(y+v.Y)*Xex+(x+v.X)] != -1){
						nodes[i].Neighbors.Add((Table[(z+v.Z)*Yex*Xex+(y+v.Y)*Xex+(x+v.X)]));
					}
				}
			}
		}
	}
}
void ANavNet::GenerateNet(TArray<FVector> Locations){
	nodes.Empty();
	for(int i =0; i<Locations.Num(); i++){
		FNode_t node;
		node.Location = Locations[i];
		node.Neighbors = {};
		nodes.Add(node);
	}
	for(int i =0; i<nodes.Num(); i++){
		for(int j =0; j<nodes.Num(); j++){
			if(i != j){
				if(!CapsuleTrace(nodes[i].Location, nodes[j].Location)){
					nodes[i].Neighbors.Add(j);
				}
			}
		}
	}
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
	locn/= VoxelSize;
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
		90,
		40,
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
double ANavNet::Infinity(){
	return 1000000000;
}
int ANavNet::NearestNode(FVector Location){
	int nearest = 0;
	double NearestDist = Infinity();
	for(int i =0; i<nodes.Num(); i++){
		double l = FVector::Distance(Location, nodes[i].Location)<NearestDist;
		if(!CapsuleTrace(Location, nodes[i].Location)){
				nearest = i;
				NearestDist = l;
		}
	}
	return nearest;
}
bool ANavNet::CapsuleTrace(FVector Start, FVector End){
	FHitResult hit;
	TArray<AActor *> ignored = {};
	return UKismetSystemLibrary::CapsuleTraceSingle(
		GetWorld(),
		Start,
		End,
		40,
		90,
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
TArray<FVector> ANavNet::Pathfind(FVector Start, FVector End){
	if(!CapsuleTrace(Start, End)){
		return {End};
	}
	int start = NearestNode(Start);
	int end = NearestNode(End);
	TArray<AStarNode_t> net = {};
	for(int i =0; i<nodes.Num(); i++){
		AStarNode_t tmp;
		for(int j =0; j<nodes[i].Neighbors.Num(); j++){
			tmp.edges.Add(nodes[i].Neighbors[j]);
			tmp.distances.Add(FVector::Distance(nodes[i].Location, nodes[nodes[i].Neighbors[j]].Location));
		}
		tmp.euc_distance = FVector::Distance(nodes[i].Location, nodes[end].Location);
		net.Add(tmp);
	}
	TArray<int> indices = UPathfinder::AStar(net,start,end);
	if(indices.Num() == 0){
		return {};
	}
	TArray<FVector> out = {};
	out.Add(nodes[start].Location);
	for(int i = 0; i<indices.Num(); i++){
		out.Add(nodes[indices[i]].Location);
	}
	out.Add(nodes[end].Location);
	out.Add(End);
	return out;	
}