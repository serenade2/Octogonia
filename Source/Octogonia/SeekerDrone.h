// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/Actor.h"
#include "SeekerDrone.generated.h"

/**
 * 
 */
UCLASS()
class OCTOGONIA_API ASeekerDrone : public AActor
{
public :
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	UBoxComponent* CollisionCoponent;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	USphereComponent* VisionSphere;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	USphereComponent* EatingSphere;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	USkeletalMeshComponent* SkeletalMesh;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	USceneComponent* eyeSocket;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	FRotator MeshRotationOffset;

	FVector Velocity;
	FVector Acceleration;
	float HP = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float MaxHP = 100;
	float Hunger = 100;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	int nextTickPrediction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float MovementSpeed;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	float SeekingWeight;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	bool isSeeking;

	UPROPERTY(BlueprintReadWrite, Category = "Flocking")
	AActor *FlockingSpawnRef;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	float OutOfBoundWeight = 0.0f;

	ASeekerDrone();
	bool CanMove = true;
	// Called every frame
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	void CalculateMovement(float DeltaSeconds);
	FVector CalculateSeeking();
	FVector CalculateOutOfBound();
	void EatNearbyDrone(float DeltaSeconds);
	void ApplyDamage(float value);
	UFUNCTION(BlueprintImplementableEvent)
	void PlayHitEffect();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayShootEffect();
	void OnDeathEvent();
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	bool IsDebugging = true;
	
};
