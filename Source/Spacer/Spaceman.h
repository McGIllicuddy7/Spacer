// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NavNet.h"
#include "Spaceman.generated.h"
struct impulse_t{
	FVector Force;
	double remaining_time;
};
UCLASS(Blueprintable)
class SPACER_API ASpaceman : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASpaceman();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION(BlueprintCallable)
	void AddPhysForce(FVector Force);
	UFUNCTION(BlueprintCallable)
	void AddPhysImpulse(FVector Force, float duration);
	UFUNCTION(BlueprintCallable,BlueprintPure)
	FVector GetPhysVelocity();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetPhysAcceleration();
	UFUNCTION(BlueprintCallable)
	void MoveDirectlyToPoint(FVector location);
	UFUNCTION(BlueprintImplementableEvent)
	void FinishedMovingDirectlyCallback();
	UFUNCTION(BlueprintCallable)
	void CancelMovement();
	UFUNCTION(BlueprintCallable)
	void MoveTo(FVector Location);
	UFUNCTION(BlueprintCallable,BlueprintPure)
	bool IsMovingToLocation();
	UFUNCTION(BlueprintImplementableEvent)
	void FinishedMoving();
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float JetpackAcceleration =1400;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float MaxVelocity = 800;
	UFUNCTION(BlueprintCallable)
	void JetpackMovementInput(FVector Direction,float magnitude);
	UFUNCTION(BlueprintCallable)
	void PhysMovementInput(FVector Direction, float magnitude);
	UFUNCTION(BlueprintCallable)
	void PhysPitchInput(float magnitude);
	UFUNCTION(BlueprintCallable)
	void PhysYawInput(float magnitude);
	UFUNCTION(BlueprintCallable)
	void PhysRollInput(float magnitude);
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float fuel = 100;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float fuelConsumptionRate = 1;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool damped = true;
	bool gravity = false;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FVector LeftHandLocation;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float LeftHandLocationAlpha;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FVector RightHandLocation;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float RightHandLocationAlpha;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool ShouldUseIk;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool PlayingAnim;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float RotationSpeed = 1;
	UFUNCTION(BlueprintCallable)
	void RotateTo(FRotator Rot);
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	ANavNet * NavMesh = NULL;
private:
	FVector physvelocity;
	FVector physacceleration;
	double mass = 100;
	TArray<impulse_t> imps;
	void PhysicsMovementHandling(float DeltaTime);
	void DirectedMovementHandling(float DeltaTime);
	void RotationHandling(float DeltaTime);
	FVector MoveToLocation;
	FVector MoveToStart;
	bool ShouldMoveTo;
	void JetpackAccelerationCallback(FVector deltav);
	bool ShouldHalt = false;
	bool ShouldRotateTo;
	FRotator TargetRotation;
	bool moving_to_location;
	int moving_index;
	TArray<FVector> MoveLocations;
	void FinishedMovingToLocation();
};
