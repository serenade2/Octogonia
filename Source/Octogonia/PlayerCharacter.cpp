// Fill out your copyright notice in the Description page of Project Settings.

#include "Octogonia.h"
#include "PlayerCharacter.h"
#include "Drone.h"
#include "SeekerDrone.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(RootComponent);

	RightHandContainer = CreateDefaultSubobject<USceneComponent>("RightHandContainer");
	//RightHandContainer->SetupAttachment(Camera);
	RightHandContainer->SetupAttachment(RootComponent);

	LeftHandContainer = CreateDefaultSubobject<USceneComponent>("LeftHandContainer");
	//LeftHandContainer->SetupAttachment(Camera);
	LeftHandContainer->SetupAttachment(RootComponent);

	RightHandMesh2 = CreateDefaultSubobject<USkeletalMeshComponent>("RightHandMesh2");
	RightHandMesh2->SetupAttachment(RightHandContainer);

	LeftHandMesh = CreateDefaultSubobject<USkeletalMeshComponent>("LeftHandMesh");
	LeftHandMesh->SetupAttachment(LeftHandContainer);

	RightHandHookPoint = CreateDefaultSubobject<USceneComponent>("RightHandHookPoint");
	RightHandHookPoint->SetupAttachment(RightHandMesh2);

	LeftHandHookPoint = CreateDefaultSubobject<USceneComponent>("LeftHandHookPoint");
	LeftHandHookPoint->SetupAttachment(LeftHandMesh);
	/**/
	EnergyLevelIndicator = CreateDefaultSubobject<UTextRenderComponent>("EnergyLevel");
	EnergyLevelIndicator->SetupAttachment(RightHandMesh2);
	EnergyLevelIndicator->SetText(FText::AsNumber(this->EnergyLevel));
	EnergyLevelIndicator->SetTextRenderColor(colorDamage);
	EnergyLevelIndicator->SetWorldSize(6.0f);
	EnergyLevel = MaxEnergyLevel;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	FVector location;
	FRotator orientation;

	if (USteamVRFunctionLibrary::GetHandPositionAndOrientation(0, EControllerHand::Left, location, orientation))
	{
		UE_LOG(LogTemp, Warning, TEXT("Left Hand Found! Position: %s Orientation: %s"), *location.ToString(), *orientation.ToString());
	}

	if (USteamVRFunctionLibrary::GetHandPositionAndOrientation(0, EControllerHand::Right, location, orientation))
	{
		UE_LOG(LogTemp, Warning, TEXT("Right Hand Found! Position: %s Orientation: %s"), *location.ToString(), *orientation.ToString());
	}

	if (USteamVRFunctionLibrary::GetHandPositionAndOrientation(0, EControllerHand::Pad, location, orientation))
	{
		UE_LOG(LogTemp, Warning, TEXT("Pad Found! Position: %s Orientation: %s"), *location.ToString(), *orientation.ToString());
	}
	WalkByCatchOrb();
	if (isShooting)
	{
		ShootLaser(RightHandHookPoint, DeltaTime);
	}

	if (isGrabbing)
	{
		TryToCatchOrb(LeftHandHookPoint, DeltaTime);
	}
	
}

void APlayerCharacter::LookRight(float axisValue)
{
	AddControllerYawInput(axisValue / 2);

}

void APlayerCharacter::LookUp(float axisValue)
{
	// Since we are changing the relative rotation, we only have to care about the pitch of the camera when looking up
	Camera->AddRelativeRotation(FRotator(axisValue, 0, 0));

	if (Camera->GetRelativeTransform().Rotator().Pitch >= 80)
	{
		Camera->SetRelativeRotation(FRotator(80, 0, 0));
	}
	else if (Camera->GetRelativeTransform().Rotator().Pitch <= -80)
	{
		Camera->SetRelativeRotation(FRotator(-80, 0, 0));
	}
}

void APlayerCharacter::WalkForward(float axisValue)
{
	AddMovementInput(GetActorForwardVector() * axisValue);
}

void APlayerCharacter::StrafeRight(float axisValue)
{
	AddMovementInput(GetActorRightVector() * axisValue);
}

void APlayerCharacter::ToogleShootingMode() 
{
	isInHealingMode = !isInHealingMode;
	ChangeGunMaterial();
	ChangeEnergyTextColor();
}

void APlayerCharacter::TriggerLeftHand()
{
	
	isGrabbing = !isGrabbing;
}

void APlayerCharacter::TriggerRightHandPressed()
{
	if (EnergyLevel > 0) {
		isShooting = true;
		IsShootingChange();
		//device.TriggerHapticPulse(10, Valve.VR.EVRButtonId.k_EButton_SteamVR_TouchPad);
		//USteamVRFunctionLibrary::
	}
}

void APlayerCharacter::TriggerRightHandReleased()
{
	isShooting = false;
	IsShootingChange();
}

void APlayerCharacter::WalkByCatchOrb()
{
	TArray<AActor*> overlappingActors;
	GetCapsuleComponent()->GetOverlappingActors(overlappingActors, AEnergyOrb::StaticClass());
	for (AActor* iterator : overlappingActors)
	{
		AEnergyOrb *Orb = Cast<AEnergyOrb>(iterator);
		Orb->Catch(GetCapsuleComponent(), OrbCatchSpeed);
		Orb->OnOrbCaught().AddUObject(this, &APlayerCharacter::ModifyEnergy);
		UE_LOG(LogTemp, Warning, TEXT("FOUND ENERGY ORB!!"));
	}
}

void APlayerCharacter::TryToCatchOrb(USceneComponent *handSocket,float DeltaTime)
{
	FVector startLocation = handSocket->GetComponentLocation()+handSocket->GetComponentVelocity();
	FVector endLocation = handSocket->GetComponentLocation() + handSocket->GetUpVector()*EnergyOrbGrabDistance;
	FVector grabVector = (endLocation - startLocation).GetSafeNormal();
	// Afficher pendant 1.1 frame ( un peu plus de temps qu'une frame)
	DrawDebugCone(GetWorld(), startLocation, grabVector, EnergyOrbGrabDistance, OrbCatchAngle, OrbCatchAngle, 10, FColor::Green, true, DeltaTime*1.1);

	for (TActorIterator<AEnergyOrb> OrbIterator(GetWorld()); OrbIterator; ++OrbIterator)
	{
		AEnergyOrb *Orb = *OrbIterator;
		if (!Orb->IsAlreadyCaught())
		{
			FVector lineTo = (Orb->GetActorLocation() - startLocation).GetSafeNormal();
			float distanceSquared = FVector::DistSquared(Orb->GetActorLocation(), startLocation);
			if (distanceSquared <= EnergyOrbGrabDistance*EnergyOrbGrabDistance &&  acos(FVector::DotProduct(lineTo, grabVector)) <= OrbCatchAngle)
			{
				Orb->Catch(handSocket, OrbCatchSpeed);
				Orb->OnOrbCaught().AddUObject(this, &APlayerCharacter::ModifyEnergy);
				UE_LOG(LogTemp, Warning, TEXT("FOUND ENERGY ORB!!"));
			}
		}
	}
	// Exemple

}

void APlayerCharacter::ShootLaser(USceneComponent *handSocket, float DeltaTime)
{

	FVector startLocation = handSocket->GetComponentLocation();
	FVector endLocation = handSocket->GetComponentLocation() + handSocket->GetUpVector()*LaserDistance;

	if (EnergyLevel > 0)
	{
		
		isInHealingMode = false;
		FHitResult hitResult;
		GetWorld()->LineTraceSingleByChannel(hitResult,
			startLocation,
			endLocation,
			ECC_Visibility);
			
			//0DrawDebugLine(GetWorld(), startLocation, endLocation, colorDamage, true, DeltaTime*1.1, 0, 2);
			//ShootParticleEffect(startLocation,endLocation,colorDamage);

		
		if (hitResult.GetActor() && hitResult.GetActor()->IsA(ADrone::StaticClass()))
		{
			ADrone* Drone = Cast<ADrone>(hitResult.GetActor());

			if (!Drone->IsFriendly)
			{
				Drone->DamageDrone(LaserDPF*DeltaTime, hitResult.ImpactPoint, (endLocation - startLocation).GetSafeNormal(), 1000);
				
			}
			else
			{

				isInHealingMode = true;
				Drone->RestoreEnergy(LaserHPF*DeltaTime);
			}
		}

		if (hitResult.GetActor() && hitResult.GetActor()->IsA(ASeekerDrone::StaticClass()))
		{
			ASeekerDrone* Seeker = Cast<ASeekerDrone>(hitResult.GetActor());
			Seeker->ApplyDamage(LaserDPF*DeltaTime);
			
	
		}
		if (hitResult.GetComponent() && hitResult.GetComponent()->IsA(UDestructibleComponent::StaticClass()))
		{
			UDestructibleComponent* Destructible = Cast<UDestructibleComponent>(hitResult.GetComponent());
			Destructible->ApplyDamage(LaserDPF*DeltaTime, hitResult.ImpactPoint, (endLocation - startLocation).GetSafeNormal(), 1000);
			
		}
		if (!isInHealingMode)
		{
			ChangeGunMaterial();
			ChangeEnergyTextColor();
			DrawDebugLine(GetWorld(), startLocation, endLocation, colorDamage, true, DeltaTime*1.1, 0, 2);
		}
		else
		{
			ChangeGunMaterial();
			ChangeEnergyTextColor();
			DrawDebugLine(GetWorld(), startLocation, endLocation, colorHealing, true, DeltaTime*1.1, 0, 2);
		}
		ModifyEnergy(-LaserEnergyDrain);
	}
	else {
		isShooting = false;
		IsShootingChange();
	}
}
void APlayerCharacter::ModifyEnergy(float value)
{
	this->EnergyLevel += value;
	if (EnergyLevel > MaxEnergyLevel) {
		EnergyLevel = MaxEnergyLevel;
	}
	EnergyLevelIndicator->SetText(FText::AsNumber(this->EnergyLevel));
	ChangeGunMaterial();
}

void APlayerCharacter::ChangeLeftHandOrientation(FRotator newRotation)
{
	LeftHandContainer->SetRelativeRotation(newRotation);
}

void APlayerCharacter::ChangeLeftHandPosition(FVector newPosition)
{
	if(LeftHandContainer != nullptr)
		LeftHandContainer->SetRelativeLocation(newPosition);
}

void APlayerCharacter::ChangeRightHandOrientation(FRotator newRotation)
{
	RightHandContainer->SetRelativeRotation(newRotation);
}

void APlayerCharacter::ChangeRightHandPosition(FVector newPosition)
{
	RightHandContainer->SetRelativeLocation(newPosition);
}

void APlayerCharacter::ChangeEnergyTextColor() 
{
	if (!isInHealingMode)
	{
		EnergyLevelIndicator->SetTextRenderColor(colorDamage);
	}
	else
	{
		EnergyLevelIndicator->SetTextRenderColor(colorHealing);
	}
}