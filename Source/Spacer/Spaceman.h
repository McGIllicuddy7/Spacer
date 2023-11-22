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
private:
	FVector physvelocity;
	FVector physacceleration;
	double mass = 100;
	bool damped = true;
	TArray<impulse_t> imps;
};
