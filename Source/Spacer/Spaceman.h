// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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
	UFUNCTION(BlueprintCallable)
	void CancelMovement();
	UFUNCTION(BlueprintCallable,BlueprintPure)
	bool IsMovingToLocation();
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float JetpackAcceleration =1400;
	UFUNCTION(BlueprintCallable)
	void JetpackMovementInput(FVector Direction,float magnitude);
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float fuel = 100;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float fuelConsumptionRate = 1;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool damped = true;
private:
	FVector physvelocity;
	FVector physacceleration;
	double mass = 100;
	TArray<impulse_t> imps;
	void PhysicsMovementHandling(float DeltaTime);
	void DirectedMovementHandling(float DeltaTime);
	FVector MoveToLocation;
	FVector MoveToStart;
	bool ShouldMoveTo;
	void JetpackAccelerationCallback(FVector deltav);
};
