// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "FlockingZone.generated.h"

UCLASS()
class OCTOGONIA_API AFlockingZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlockingZone();

//Components
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	USphereComponent* SpawnZone;

//Editable Variable
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	int ZoneRadius;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	int nbBasicDrone;

//Methods
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	FVector getRandomSpawnLocation(int radius);
};