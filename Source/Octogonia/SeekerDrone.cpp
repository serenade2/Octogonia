// Fill out your copyright notice in the Description page of Project Settings.

#include "Octogonia.h"
#include "SeekerDrone.h"
#include "Drone.h"
ASeekerDrone::ASeekerDrone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SeekingWeight = 1.0f;
	nextTickPrediction = 5;
	isSeeking = false;

	CollisionCoponent = CreateDefaultSubobject<UBoxComponent>("Collision Component", false);
	RootComponent = CollisionCoponent;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Body", false);
	SkeletalMesh->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

	EatingSphere = CreateDefaultSubobject<USphereComponent>("Eating Sphere ", false);
	EatingSphere->SetSphereRadius(200, true);
	EatingSphere->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

	VisionSphere = CreateDefaultSubobject<USphereComponent>("VisionSphere", false);
	VisionSphere->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false));
	VisionSphere->SetSphereRadius(400, true);

	eyeSocket = CreateDefaultSubobject<USceneComponent>("Eye Socket", false);
	eyeSocket->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
}
void ASeekerDrone::EatNearbyDrone(float DeltaSeconds)
{
	if (Hunger > 100)
	{
		TArray<AActor*> Food;
		EatingSphere->GetOverlappingActors(Food, ADrone::StaticClass());
		FVector startLocation = eyeSocket->GetComponentLocation();
		for (AActor* droneToEat : Food)
		{
			PlayShootEffect();
			ADrone* drone = Cast<ADrone>(droneToEat);
			FVector endLocation = drone->GetActorLocation();
			DrawDebugLine(GetWorld(), startLocation, endLocation, FColor::Red, true, DeltaSeconds*1.1, 0, 10);
			FHitResult hitResult;
			GetWorld()->LineTraceSingleByChannel(hitResult,
				startLocation,
				drone->GetActorLocation(),
				ECC_Visibility);


			if (hitResult.GetActor() && hitResult.GetActor()->IsA(ADrone::StaticClass()))
			{
				ADrone* drone = Cast<ADrone>(hitResult.GetActor());
				if (!drone->IsFriendly) {
					drone->DamageDrone(5, hitResult.ImpactPoint, (endLocation - startLocation).GetSafeNormal(), 1000);
					this->Hunger -= drone->FoodValue;
				}
			}

			break;
		}
	}

}
void ASeekerDrone::CalculateMovement(float DeltaSeconds) {
	FHitResult Hit(1.f);
	if (CanMove)
	{
		//Calculate next position
		Acceleration = FVector(0, 0, 0);
		Acceleration += CalculateSeeking();
		Acceleration += CalculateOutOfBound();
		Velocity += Acceleration;


		//changer mouvementSpeed with get speed
		if (Velocity.Size() > MovementSpeed) {
			Velocity = Velocity.GetSafeNormal() * MovementSpeed;
			Velocity.Size();
		}

		FVector NewLocation = GetActorLocation() + Velocity*DeltaSeconds;

		//Calculate next rotation
		FVector velocityNormalize = Velocity.GetSafeNormal();
		if (IsDebugging)
			DrawDebugLine(GetWorld(), this->GetActorLocation(), this->GetActorLocation() + velocityNormalize * 100, FColor::Green, true, 0.02, 0, 2);
		FRotator NewRotation = velocityNormalize.Rotation();
		NewRotation = FRotator(NewRotation.Pitch, NewRotation.Yaw, 0);
		if (IsDebugging)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Drone's velocity is %s"), *velocityNormalize.ToString()));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Drone's Rotation is %s"), *NewRotation.ToString()));
		}
		RootComponent->SetWorldLocation(NewLocation, true, &Hit);
		RootComponent->SetRelativeRotation(NewRotation, true, &Hit);


		if (Hit.IsValidBlockingHit())
		{

			const FVector Normal = Hit.Normal.GetSafeNormal();
			const FVector Deflection = -Velocity;//FVector::VectorPlaneProject(Velocity, Normal) * (1.f - Hit.Time);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Drone's deflection is %s"), *Deflection.ToString()));
			RootComponent->MoveComponent(Deflection, NewRotation, true);
		}


	}
}
FVector ASeekerDrone::CalculateSeeking() {
	TArray<AActor*> Neighborhood;
	VisionSphere->GetOverlappingActors(Neighborhood, ADrone::StaticClass());

	FVector result = FVector(0, 0, 0);
	AActor* closestNeigbor = NULL;
	float closestDistance = INFINITE;



	for (AActor* neighbor : Neighborhood) {
		if (neighbor->IsA(ADrone::StaticClass()) && neighbor != this) {
			ADrone *Drone = Cast<ADrone>(neighbor);
			if (!Drone->IsFriendly)
			{
				FVector distanceVector = this->GetActorLocation() - Drone->GetActorLocation();
				if (distanceVector.Size() < closestDistance) {
					closestDistance = distanceVector.Size();
					closestNeigbor = Drone;
				}
			}
			
		}
	}

	if (closestNeigbor != NULL) {
		FVector seekingTargetPosition = closestNeigbor->GetActorLocation() + closestNeigbor->GetVelocity() * nextTickPrediction;
		FVector seekingTargetVector = seekingTargetPosition - this->GetActorLocation();
		seekingTargetVector.Normalize();
		result = seekingTargetVector * SeekingWeight;
	}

	return result;
}
void ASeekerDrone::BeginPlay()
{
	Super::BeginPlay();
	//MovementVector = GetActorRotation().Vector().GetSafeNormal();

	//Generate random starting rotation velocity
	FRandomStream rand;
	rand.GenerateNewSeed();
	//Velocity = FVector(0, 0, -1);
	Velocity = rand.GetUnitVector();
	Velocity *= MovementSpeed;
	HP = MaxHP;
}
void ASeekerDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CalculateMovement(DeltaTime);
	EatNearbyDrone(DeltaTime);
	Hunger += DeltaTime;
}
void ASeekerDrone::ApplyDamage(float value)
{
	HP -= value;
	if (HP <= 0)
	{
		SkeletalMesh->SetSimulatePhysics(true);
		this->SetLifeSpan(5);
	}
		
}
FVector ASeekerDrone::CalculateOutOfBound() {
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