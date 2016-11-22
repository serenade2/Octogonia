// Fill out your copyright notice in the Description page of Project Settings.

#include "Octogonia.h"
#include "Drone.h"
#include "PlayerCharacter.h"
#include "math.h"


// Sets default values
ADrone::ADrone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SeperationWeight = 0.1f;
	CohesionWeight = 0.01f;
	AlignmentWeight = 0.1f;
	MovementSpeed = 1.0f;
	OutOfBoundWeight = 0.05f;



	ColisionComponent = CreateDefaultSubobject<USphereComponent>("Root", false);
	ColisionComponent->SetSphereRadius(200, true);
	RootComponent = ColisionComponent;

	Mesh = CreateDefaultSubobject<UDestructibleComponent>("Mesh", false);
	Mesh->SetupAttachment(RootComponent);
	Mesh->OnComponentFracture.AddDynamic(this, &ADrone::DestroyDrone);

	VisionSphere = CreateDefaultSubobject<USphereComponent>("VisionSphere", false);
	VisionSphere->SetupAttachment(RootComponent);
	VisionSphere->SetSphereRadius(400, true);
}

// Called when the game starts or when spawned
void ADrone::BeginPlay()
{
	Super::BeginPlay();
	//MovementVector = GetActorRotation().Vector().GetSafeNormal();

	//Generate random starting rotation velocity
	FRandomStream rand;
	rand.GenerateNewSeed();
	//Velocity = FVector(0, 0, -1);
	Velocity = rand.GetUnitVector();
	Velocity *= MovementSpeed;
	startInterval = 0.f;
	SetSpeed(MaxSpeed);
	SetEnergy(MaxEnergy);
}

void ADrone::DestroyDrone(const FVector& hit1, const FVector& hit2)
{
	CanMove = false;
	this->SetLifeSpan(2);
	//MovementVector = GetActorRotation().Vector().GetSafeNormal();
}
void  ADrone::DamageDrone(float damage, const FVector& hitlocation, const FVector& hitDirection, float impulse)
{
	Mesh->ApplyDamage(damage, hitlocation, hitDirection, impulse);
}
// Called every frame
void ADrone::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	
	
	if (IsFriendly) 
	{
		ConsumeEnergy(DeltaTime);
	}
	else 
	{
		CalculateMovement(DeltaTime);
	}
	
	ComputeSpeed(DeltaTime);
}

void ADrone::CalculateMovement(float DeltaSeconds) {
	FHitResult Hit(1.f);
	if (CanMove)
	{
		//Calculate next position
		Acceleration = FVector(0, 0, 0);
		Acceleration += CalculateOutOfBound();
		Acceleration += CalculateFlocking();
		
		Velocity += Acceleration;


		//changer mouvementSpeed with get speed
		if(Velocity.Size() > GetSpeed() ) {
			Velocity = Velocity.GetSafeNormal() * GetSpeed();
			Velocity.Size();
		}

		FVector NewLocation =  Velocity*DeltaSeconds;

		//Calculate next rotation
		FVector velocityNormalize = Velocity.GetSafeNormal();
		if (IsDebugging)
			DrawDebugLine(GetWorld(), this->GetActorLocation(), this->GetActorLocation() + velocityNormalize * 100, FColor::Green, true, 0.02, 0, 2);
		FRotator NewRotation = velocityNormalize.Rotation();
		NewRotation = FRotator(NewRotation.Pitch, NewRotation.Yaw, 0);
		NewRotation += MeshRotationOffset;
		if (IsDebugging)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Drone's velocity is %s"), *velocityNormalize.ToString()));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Drone's Rotation is %s"), *NewRotation.ToString()));
		}
		RootComponent->MoveComponent(NewLocation, NewRotation, true);

		
		if (Hit.IsValidBlockingHit())
		{

			const FVector Normal = Hit.Normal.GetSafeNormal();
			const FVector Deflection = FVector::VectorPlaneProject(Velocity, Normal) * (1.f - Hit.Time);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Drone's deflection is %s"), *Deflection.ToString()));
			DrawDebugLine(GetWorld(), this->GetActorLocation(), this->GetActorLocation() + Deflection * 100, FColor::Green, true, 0.02, 0, 2);
		
			RootComponent->MoveComponent(Deflection, NewRotation, true);
		}
		
		
	}
}

FVector ADrone::CalculateOutOfBound() {
	FVector result = FVector(0, 0, 0);
	FVector origin = FVector(0, 0, 0);
	FVector BoundsExtent = FVector(0, 0, 0);

	if (FlockingSpawnRef != NULL) {
		FlockingSpawnRef->GetActorBounds(false, origin, BoundsExtent);
		FVector ActorToOrigin = origin - this->GetActorLocation();
		USphereComponent* zone = FlockingSpawnRef->FindComponentByClass<USphereComponent>();
		
		if (IsDebugging)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Drone's Rotation is %s"), *origin.ToString()));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Drone's Rotation is %s"), *this->GetActorLocation().ToString()));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Drone's Rotation is %s"), *ActorToOrigin.ToString()));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Silver, FString::Printf(TEXT("vector = %f"), ActorToOrigin.Size()));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("vector = %f"), (zone->GetScaledSphereRadius())));
		}

		if (ActorToOrigin.Size() > zone->GetScaledSphereRadius()) {
			result = ActorToOrigin * OutOfBoundWeight;
		}
		//result = ActorToOrigin * OutOfBoundWeight;
	}

	return result;
}


FVector ADrone::CalculateFlocking()
{
	FVector result = FVector(0, 0, 0);
	Cohesion = FVector(0, 0, 0);
	Seperation = FVector(0, 0, 0);
	Alignment = FVector(0, 0, 0);
	int nbDroneNeighbor = 0;
	int nbDroneSeparated = 0;

	TArray<AActor*> Neighborhood;

	VisionSphere->GetOverlappingActors(Neighborhood, ADrone::StaticClass());

	if (Neighborhood.Num() > 0 )
	{
		for (AActor* neighbor : Neighborhood)
		{
			if (neighbor->IsA(ADrone::StaticClass()) && neighbor != this) {
				ADrone *Drone = Cast<ADrone>(neighbor);
				Cohesion += Drone->GetActorLocation();
				Alignment += Drone->Velocity;
				
				FVector separationVector = this->GetActorLocation() - Drone->GetActorLocation();
				if (separationVector.Size() < (VisionSphere->GetScaledSphereRadius()*SeparationDistRatio)) {
					Seperation += separationVector;
					nbDroneSeparated++;
				}
				
				nbDroneNeighbor++;
			}
		}
		if (nbDroneNeighbor > 0) {
			Cohesion /= nbDroneNeighbor;
			Cohesion = Cohesion - this->GetActorLocation();
			Cohesion.Normalize();

			Alignment /= nbDroneNeighbor;
			Alignment.Normalize();

			if (nbDroneSeparated > 0) {
				Seperation /= nbDroneSeparated;
				Seperation.Normalize();
			}
		}
	}

	result = Cohesion * CohesionWeight + Alignment * AlignmentWeight + Seperation * SeperationWeight;

	return result;
}



float ADrone::GetSpeed() const
{
	return MovementSpeed;
	//return CurrentSpeed;
}
void ADrone::ConsumeEnergy(float DeltaTime) 
{
	//UE_LOG(LogTemp, Warning, TEXT("ConsumeEnergy"));
	startInterval += DeltaTime;

	if(startInterval >= ConsumeInterval && CurrentEnergy > 0) 
	{
		//CurrentEnergy -= CostEnergy;
		SetEnergy(CurrentEnergy-CostEnergy);
		startInterval = 0.0f;
	}
}
// this function compute the speed of the drone
void ADrone::ComputeSpeed(float DeltaTime) 
{
	//UE_LOG(LogTemp, Warning, TEXT("ComputeSpeed"));
	// first check how much the speed have progressed
	/*if(CurrentEnergy > 0) 
	{
		SetSpeed(MovementSpeed + CurrentEnergy);
	}
	else
	{
		SetSpeed(0);
	}*/
}

void ADrone::SetEnergy(int value) 
{
	if(value > MaxEnergy)
	{
		CurrentEnergy = MaxEnergy;
	}
	else if(CurrentEnergy < 0) 
	{
		CurrentEnergy = 0;
	}
	else 
	{
		CurrentEnergy = value;
	}
}

void ADrone::SetSpeed(float Speed) 
{
	if(Speed > MaxSpeed) 
	{
		CurrentSpeed = MaxSpeed;
	} 
	else if(CurrentSpeed < 0) 
	{
		CurrentSpeed = 0;
	} 
	else 
	{
		CurrentSpeed = Speed;
	}
}

void ADrone::RestoreEnergy(float energyAmount) 
{
	CurrentEnergy += energyAmount;
}
