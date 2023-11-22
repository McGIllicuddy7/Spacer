// Fill out your copyright notice in the Description page of Project Settings.

 	

#include "Kismet/KismetSystemLibrary.h"
#include "Spaceman.h"


// Sets default values
ASpaceman::ASpaceman()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpaceman::BeginPlay()
{
	Super::BeginPlay();
	
}
void ASpaceman::PhysicsMovementHandling(float DeltaTime){
		for(int i = 0; i<imps.Num(); i++){
		physacceleration+= imps[i].Force;
		imps[i].remaining_time-=DeltaTime;
		if(imps[i].remaining_time <=0){
			imps.RemoveAt(i);
			i--;
		}
	}	
	physvelocity += physacceleration*DeltaTime;
	if(physvelocity.Size()>700){
		physvelocity.Normalize();
		physvelocity*= 700;
	}
	bool accelerated = physacceleration == (FVector){0,0,0};
	physacceleration = {0,0,0};
	FHitResult result;
	FVector new_loc = GetActorLocation()+physvelocity*DeltaTime+1/2*physacceleration*DeltaTime*DeltaTime;
	bool hit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), GetActorLocation(), new_loc, 34, 88, ETraceTypeQuery::TraceTypeQuery1, false, {this},EDrawDebugTrace::None, result, true);
	if(hit){
		SetActorLocation(result.Location+result.Normal);
		FVector old = physvelocity;
		double vel = old.Size();
		old = old.GetSafeNormal();
		physvelocity.MirrorByVector(result.Normal);
		double v = 1-FVector::DotProduct(old,result.Normal);
		if(v>1){
			v = 1;
		}
		if(v<0){
			v = 0;
		}
		physvelocity *= v;
		result.Component->AddImpulse(old*v*vel*50);
	}
	else{
		SetActorLocation(new_loc);
	}
	if(accelerated&& damped){
		if(physvelocity.Size()!= 0){
			FVector dv = physvelocity;
			dv.Normalize();
			physvelocity -=dv*DeltaTime*JetpackAcceleration;
			JetpackAccelerationCallback(dv*DeltaTime*JetpackAcceleration);
			if(physvelocity.Size()>0){
				FVector f = physvelocity;
				f.Normalize();
				if(FVector::DotProduct(dv, f)<0){
					physvelocity = {0,0,0};
				}
			}
		}
	}
	else if(!damped){
		if(hit){
			physvelocity += result.Normal*50;
			physvelocity/=2;
		}
	}
}
void ASpaceman::JetpackAccelerationCallback(FVector deltav){
	double f = deltav.Size();
	f /= 36000*fuelConsumptionRate;
	fuel -= f*GetWorld()->DeltaTimeSeconds;
}
void ASpaceman::DirectedMovementHandling(float DeltaTime){
	
}
void ASpaceman::JetpackMovementInput(FVector Direction,float magnitude){
	this->AddPhysForce(Direction*magnitude*JetpackAcceleration*100);
	JetpackAccelerationCallback(this->physvelocity*Direction.Size()*magnitude*JetpackAcceleration);
}
// Called every frame
void ASpaceman::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DirectedMovementHandling(DeltaTime);
	PhysicsMovementHandling(DeltaTime);
}

// Called to bind functionality to input
void ASpaceman::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASpaceman::AddPhysForce(FVector Force){
	physacceleration+= Force/mass;
}
void ASpaceman::AddPhysImpulse(FVector Force, float duration){
	impulse_t imp = {Force, duration};
	imps.Add(imp);
}
FVector ASpaceman:: GetPhysVelocity(){
	return physvelocity;	
}
FVector ASpaceman::GetPhysAcceleration(){
	return physacceleration;
}

void ASpaceman::MoveDirectlyToPoint(FVector location){

}

void ASpaceman::CancelMovement(){

}

bool ASpaceman::IsMovingToLocation(){
	return false;
}