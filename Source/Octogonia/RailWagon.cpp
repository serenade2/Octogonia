// Fill out your copyright notice in the Description page of Project Settings.

#include "Octogonia.h"
#include "RailWagon.h"
#include "EngineUtils.h"
#include "Drone.h"
#include "EnergyOrb.h"

// Sets default values
ARailWagon::ARailWagon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	AnchorPoint = CreateDefaultSubobject<USceneComponent>("AnchorPoint");
	AnchorPoint->SetupAttachment(RootComponent);

	DroneAverageSpeedHeuristic = 0.9f;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ARailWagon::BeginPlay()
{
	Super::BeginPlay();

	UpdateWagonLocation();
}

// Called every frame
void ARailWagon::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	ComputeAverageSpeed(DeltaTime);
	UpdateWagonLocation();
}

//void ARailWagon::ComputeAverageSpeed(float DeltaTime) 
//{
//	float averageSpeed = 0;
//	int numberOfDrones = 0;
//	float droneSpeed = 0;
//	// return the iterator of all the drone of the actual world
//	/*for(TActorIterator<ADrone> ActorItr(GetWorld()); ActorItr; ++ActorItr) 
//	{
//		if(ActorItr->GetSpeed() > 0)
//			averageSpeed += ActorItr->GetSpeed();
//		numberOfDrones++;
//	}*/
//
//	float averageEnergy = 0;
//	for (TActorIterator<ADrone> ActorItr(GetWorld()); ActorItr; ++ActorItr)
//	{
//		// only retrieve the drone that 
//		if(ActorItr->IsFriendly)
//		{
//			averageEnergy += ActorItr->GetEnergy();
//			numberOfDrones++;
//		}
//	}
//
//	averageEnergy /= numberOfDrones;
//	averageSpeed *= DroneAverageSpeedHeuristic;
//
//	//UE_LOG(LogTemp, Warning, TEXT("ComputeAverageSpeed: %f"), averageSpeed);*/
//	//splineDistance += averageSpeed * DeltaTime;
//
//	splineDistance += averageEnergy * DeltaTime;
//}

void ARailWagon::ComputeAverageSpeed(float DeltaTime)
{
	float averageSpeed = 0;
	int numberOfDrones = 0;
	float droneSpeed = 0;
	// return the iterator of all the drone of the actual world
	/*for(TActorIterator<ADrone> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
	if(ActorItr->GetSpeed() > 0)
	averageSpeed += ActorItr->GetSpeed();
	numberOfDrones++;
	}*/

	float averageEnergy = 0;
	for (TActorIterator<ADrone> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->IsFriendly) 
		{
			averageEnergy += ActorItr->GetEnergy();
			numberOfDrones++;
		}
	}
	
	averageEnergy /= numberOfDrones;
	averageSpeed = log(averageEnergy + 1) * EnergySpeedFactor + MinimalSpeed;
	//UE_LOG(LogTemp, Warning, TEXT("ComputeAverageSpeed: %f"), averageSpeed);
	splineDistance += averageSpeed * DeltaTime;
}

void ARailWagon::UpdateWagonLocation()
{
	// If a RailSpline has been linked, we can safely position our wagon on it
	if (RailSpline)
	{
		auto totalLength = RailSpline->GetSplineComponent()->GetSplineLength();
		if (splineDistance > totalLength)
			splineDistance = totalLength;

		FVector location = RailSpline->GetSplineComponent()->GetLocationAtDistanceAlongSpline(splineDistance, ESplineCoordinateSpace::World);
		FVector direction = RailSpline->GetSplineComponent()->GetDirectionAtDistanceAlongSpline(splineDistance, ESplineCoordinateSpace::World);

		SetActorLocation(location);
		SetActorRotation(direction.Rotation());

		//Spawning Orbs
		if((long)splineDistance % 100 == 0) {
			FVector orbLocation = RailSpline->GetSplineComponent()->GetLocationAtDistanceAlongSpline(splineDistance+8000, ESplineCoordinateSpace::World);
			RailSpline->SpawnOrbs(orbLocation);
		}
	}
}
