// Fill out your copyright notice in the Description page of Project Settings.

#include "Octogonia.h"
#include "EnergyOrb.h"

//Test PO
// Sets default values
AEnergyOrb::AEnergyOrb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	EnergyValue = 10;

}

// Called when the game starts or when spawned
void AEnergyOrb::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnergyOrb::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	switch (currentState)
	{
	case EEnergyOrbState::BeingCaught:
		TranslateTowardHand(DeltaTime);
		break;
	}
}

void AEnergyOrb::Catch(USceneComponent *catchHandSocket, float speed)
{
	if (currentState != EEnergyOrbState::Idle) return;

	CatchHandSocket = catchHandSocket;
	LerpSpeed = speed;
	CatchStartLocation = GetActorLocation();
	CatchStartScale = GetActorScale();

	currentState = EEnergyOrbState::BeingCaught;
	LerpRatio = 0.f;
}

void AEnergyOrb::TranslateTowardHand(float deltaTime)
{
	FVector endLocation = CatchHandSocket->GetComponentLocation();

	LerpRatio += LerpSpeed / (CatchStartLocation - endLocation).Size();
	float clampedRatio = FMath::Clamp(LerpRatio, 0.f, 1.f);

	SetActorLocation(FMath::Lerp(CatchStartLocation, endLocation, clampedRatio));

	FVector newScale = FMath::Lerp(CatchStartScale, CatchEndScale, clampedRatio);

	SetActorScale3D(newScale);

	if (LerpRatio >= 1.f)
	{
		currentState = EEnergyOrbState::Caught;
		OrbCaught.Broadcast(EnergyValue);
		OrbCaught.Clear();
		OnOrbCaughtBP();
		this->Destroy();
	}
}

bool AEnergyOrb::IsAlreadyCaught()
{
	return currentState == EEnergyOrbState::BeingCaught || currentState == EEnergyOrbState::Caught;
}
