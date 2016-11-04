// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Drone.generated.h"

UCLASS()
class OCTOGONIA_API ADrone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADrone();

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	USphereComponent* ColisionComponent;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	UDestructibleComponent* Mesh;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	USphereComponent* VisionSphere;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (AllowPrivateAccess))
	float MovementSpeed;

	FVector Seperation;
	FVector Alignment;
	FVector Cohesion;

	FVector Velocity;
	FVector Acceleration;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Rotation")
	FRotator MeshRotationOffset;

	//UPROPERTY(BlueprintReadWrite,Category="Flocking")
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess), Category = "Flocking")
	AActor *FlockingSpawnRef;


	UPROPERTY(BlueprintReadWrite,EditAnywhere, meta = (AllowPrivateAccess))
	float SeperationWeight;
	UPROPERTY(BlueprintReadWrite,EditAnywhere, meta = (AllowPrivateAccess))
	float AlignmentWeight;
	UPROPERTY(BlueprintReadWrite,EditAnywhere, meta = (AllowPrivateAccess))
	float CohesionWeight;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	float OutOfBoundWeight = 0.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	float SeparationDistRatio;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	int MaxEnergy = 100;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	float FoodValue = 10.0f;
	// the remaining energy
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	int CurrentEnergy = 100;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	float MaxSpeed = 200000;
	// the actual speed
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	float CurrentSpeed = 0.0f;
	// the amount of energy consumed at each interval 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	float CostEnergy = 5;
	// the interval value that will determine when the current energy will be updated
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	float ConsumeInterval = 2;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool CanMove = true;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	bool IsDebugging = true;
	// specify if it's a friendly drone
	UPROPERTY(BlueprintReadWrite,EditAnywhere, meta = (AllowPrivateAccess))
	bool IsFriendly = false;

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	void CalculateMovement(float DeltaSeconds);
	FVector CalculateFlocking();
	FVector CalculateOutOfBound();

	UFUNCTION()
	void DestroyDrone(const FVector& hit1, const FVector& hit2);

	void DamageDrone(float damage, const FVector& hitlocation, const FVector& hitDirection, float impulse);
	
	UFUNCTION(BlueprintImplementableEvent)
	void PlayHitEffect();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayDeathEffect();

	UFUNCTION()
	float GetSpeed() const;
	UFUNCTION()
	void ConsumeEnergy(float DeltaTime); //the amount of energy that would be reduced over time*/
	UFUNCTION()
	void SetSpeed(float Speed);
	int GetEnergy() { return  CurrentEnergy; };
	void SetEnergy(int value);

	UFUNCTION()
	void RestoreEnergy(float energyAmount);
private:
	UFUNCTION()
	void ComputeSpeed(float DeltaTime);
	float startInterval;
	
	
};
