// Fill out your copyright notice in the Description page of Project Settings.

#include "Octogonia.h"
#include "FlockingZone.h"


// Sets default values
AFlockingZone::AFlockingZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ZoneRadius = 4000;

	SpawnZone = CreateDefaultSubobject<USphereComponent>("Root", false);
	SpawnZone->SetSphereRadius(ZoneRadius, true);
	RootComponent = SpawnZone;
}

// Called when the game starts or when spawned
void AFlockingZone::BeginPlay()
{
	Super::BeginPlay();

	for (int i = nbBasicDrone; i < 20; i++) {
		//DroneBP* NewActor = UFunctionLibrary::SpawnBP<DroneBP>(GetWorld(), TheActorBluePrint, SpawnLoc, SpawnRot);
	}

	this->GetActorLocation();
}

// Called every frame
void AFlockingZone::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

FVector getRandomSpawnLocation(int radius) {
	//FRandomStream rand;
	//rand.GenerateNewSeed();

	//FVector randomUnitVector = rand.GetUnitVector();

	FVector result;// = randomUnitVector * radius;

	return result;
}