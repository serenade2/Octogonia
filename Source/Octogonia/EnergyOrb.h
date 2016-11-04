// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "EnergyOrb.generated.h"

UCLASS()
class OCTOGONIA_API AEnergyOrb : public AActor
{
	GENERATED_BODY()
	
	
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	FVector CatchEndScale = FVector(0.08f, 0.08f, 0.08f);

public:	
	// Sets default values for this actor's properties
	AEnergyOrb();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnOrbCaughtBP();

	void Catch(USceneComponent *attachSocket, float duration);

	bool IsAlreadyCaught();

	DECLARE_EVENT_OneParam(AEnergyOrb, FOrbCaught, float)
	FOrbCaught& OnOrbCaught() { return OrbCaught; }
private:
	enum class EEnergyOrbState { Idle, BeingCaught, Caught };
	EEnergyOrbState currentState;

	float LerpRatio;
	float LerpSpeed;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float EnergyValue;

	FVector CatchStartLocation;
	USceneComponent *CatchHandSocket;
	FVector CatchStartScale;
	FOrbCaught OrbCaught;
	void TranslateTowardHand(float deltaTime);
};
