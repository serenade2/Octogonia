// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CustomSpline.h"
#include "RailWagon.generated.h"

UCLASS()
class OCTOGONIA_API ARailWagon : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, meta=(AllowPrivateAccess))
	USceneComponent *Root;

	// Anchor point used to hook the wagon to the rails
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	USceneComponent *AnchorPoint;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	UStaticMeshComponent *Mesh;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float DroneAverageSpeedHeuristic;

	// The acceleration has to be very smooth, otherwise the players might suffer from motion sickness
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float WagonAcceleration;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float EnergySpeedFactor = 500;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float MinimalSpeed = 500;

public:	
	UPROPERTY(BlueprintReadWrite)
	ACustomSpline *RailSpline;

	// Sets default values for this actor's properties
	ARailWagon();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable,Category = Mesh)
	UStaticMeshComponent* GetMesh() { return Mesh; }
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	UFUNCTION()
	void ComputeAverageSpeed(float DeltaTime);
private:
	float splineDistance;

	void UpdateWagonLocation();
};
