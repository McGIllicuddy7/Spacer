// Fill out your copyright notice in the Description page of Project Settings.

 	
#include "Spaceman.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"



// Sets default values
ASpaceman::ASpaceman()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ShouldUseIk = false;

}

// Called when the game starts or when spawned
void ASpaceman::BeginPlay()
{
	Super::BeginPlay();
	// UCharacterMovementComponent*
	auto MovementComponent = GetCharacterMovement();
	gravity = MovementComponent->GetGravityZ()>0;
	if(gravity){
		bUseControllerRotationPitch = false;
	}
}
void ASpaceman::PhysicsMovementHandling(float DeltaTime){
	if(gravity){
		return;
	}
	for(int i = 0; i<imps.Num(); i++){
		physacceleration+= imps[i].Force;
		imps[i].remaining_time-=DeltaTime;
		if(imps[i].remaining_time <=0){
			imps.RemoveAt(i);
			i--;
		}
	}	
	physvelocity += physacceleration*DeltaTime;
	if(physvelocity.Size()>MaxVelocity){
		physvelocity.Normalize();
		physvelocity*= MaxVelocity;
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
	if(!ShouldMoveTo){
		if(ShouldHalt){
			if(GetPhysVelocity().Size()>100){
				FVector dv = GetPhysVelocity();
				dv.Normalize();
				PhysMovementInput(dv,-1);
			}
			else if(GetPhysVelocity().Size()>0){

			}
			else{
				ShouldHalt = false;
				FinishedMovingToLocation();
			}
		}
		return;
	}
	FVector dv = MoveToLocation-GetActorLocation();
	float dist = FVector::Dist(GetActorLocation(),MoveToLocation);
	float v = GetPhysVelocity().Size();
	float a = JetpackAcceleration;
	if(dist<100){
		ShouldMoveTo = false;
		ShouldHalt = true;
		return;
	}
	dv.Normalize();
	if(v<1){
		PhysMovementInput(dv,1);
		return;
	}
	//d = 1/2at^2+vt
	//0 = 1/2at^2+vt-d
	// t = -v+/-sqrt(v^2-4*1/2a*-d)/a by the quadtratic formula
	// t = -v+sqrt(v^2+2ad)/a
	float ttia = (-v+sqrtf(v*v+2*a*dist))/a; //time to impact speeding up
	float ttis = (-v+sqrtf(v*v-2*a*dist))/-a;//time to impact slowing down
	if(ttis>v/a){
		ShouldMoveTo = false;
		ShouldHalt = true;
	}
	else{
		PhysMovementInput(dv,1);
	}
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
	RotationHandling(DeltaTime);
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
	ShouldMoveTo = true;
	MoveToLocation = location;
	if(NavMesh){
		if(NavMesh->CapsuleTrace(GetActorLocation(),location)){
			ShouldMoveTo = false;
			MoveToLocation = {0,0,0};
			FinishedMovingDirectlyCallback();
			return;
		}
	}
	MoveToStart = GetActorLocation();
}

void ASpaceman::CancelMovement(){
	ShouldMoveTo = false;
}

bool ASpaceman::IsMovingToLocation(){
	return ShouldMoveTo;
}
void ASpaceman::PhysMovementInput(FVector Direction, float magnitude){
	if(gravity){
		AddMovementInput(Direction, magnitude);
	}
	else{
		JetpackMovementInput(Direction, magnitude);	
	}

}
void ASpaceman::PhysPitchInput(float magnitude){
	if(gravity){
		AddControllerPitchInput(-magnitude);
	}
	else{
		AddActorLocalRotation({magnitude,0,0},false);
	}

}

void ASpaceman::PhysYawInput(float magnitude){	
	if(gravity){
		AddControllerYawInput(magnitude);
	}
	else{
		AddActorLocalRotation({0,magnitude,0},false);	
	}

}

void ASpaceman::PhysRollInput(float magnitude){
	if(gravity){
		return;
	}
	else{
		AddActorLocalRotation({0,0,magnitude},false);
	}
}
void ASpaceman::RotationHandling(float DeltaTime){
	if(!ShouldRotateTo){
		return;
	}
	FRotator R = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed);
	SetActorRotation(R);
}
void ASpaceman::RotateTo(FRotator Rot){
	ShouldRotateTo = true;
	TargetRotation = Rot;
}	
void ASpaceman::MoveTo(FVector Location){
	if(!NavMesh){
		return;
	}
	MoveLocations = NavMesh->Pathfind(GetActorLocation(), Location);
	if(MoveLocations.Num()<1){
		FinishedMovingToLocation();
		return;
	}
	moving_to_location = true;
	moving_index =0;
	MoveDirectlyToPoint(MoveLocations[moving_index]);
}
void ASpaceman::FinishedMovingToLocation(){
	moving_index+=1;	
	FinishedMovingDirectlyCallback();
	if(moving_index>=MoveLocations.Num()){
		moving_to_location = false;
		moving_index = 0;
		last_idx = -1;
		FinishedMoving();
		return;
	}
	MoveDirectlyToPoint(MoveLocations[moving_index]);
}
FVector ASpaceman::GetMoveToLocation(){
	return this->MoveToLocation;
}
void ASpaceman::TakeBodyDamage(hit_location_t hit_location, float damage){
	switch (hit_location)
	{
	case Torso:{
		this->torso_health-=damage;
	}
	case Head:{
		this->torso_health-=damage*3;
	}
	case Arm_Left:{
		this->torso_health-=damage/4;
	}
	case Arm_Right:{
		this->torso_health-=damage/4;
	}
	case Leg_Left:{
		this->torso_health-=damage/4;
	}
	case Leg_Right:{
		this->torso_health-=damage/4;
	}
	default:
		break;
	}
	if(this->torso_health<=0){
		Death();
	}
}