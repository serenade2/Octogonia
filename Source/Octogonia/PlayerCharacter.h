// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "SteamVRFunctionLibrary.h"
#include "EnergyOrb.h"
#include "PlayerCharacter.generated.h"

#define ENERGY_ORB_CHANNEL ECC_GameTraceChannel1

UCLASS()
class OCTOGONIA_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	UCameraComponent *Camera;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	USceneComponent *RightHandContainer;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	USceneComponent *LeftHandContainer;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	USkeletalMeshComponent *RightHandMesh2;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	USkeletalMeshComponent *LeftHandMesh;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	USceneComponent *RightHandHookPoint;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	USceneComponent *LeftHandHookPoint;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	UTextRenderComponent *EnergyLevelIndicator;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float EnergyOrbGrabDistance = 1000.f;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float LaserDistance = 1000.f;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float OrbCatchSpeed = 100.f;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float OrbCatchAngle = 0.349066f;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float LaserDPF = 0.5f;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float LaserEnergyDrain = 0.5f;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	float LaserHPF = 0.5f;




public:
	// Sets default values for this character's properties
	APlayerCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void LookRight(float axisValue);
	void LookUp(float axisValue);
	void WalkForward(float axisValue);
	void StrafeRight(float axisValue);
	void TriggerLeftHand();
	void TriggerRightHandPressed();
	void TriggerRightHandReleased();

	void ChangeLeftHandOrientation(FRotator newRotation);
	void ChangeLeftHandPosition(FVector newPosition);
	void ChangeRightHandOrientation(FRotator newRotation);
	void ChangeRightHandPosition(FVector newPosition);
	void ToogleShootingMode();

	UFUNCTION()
	void ModifyEnergy(float value);
	UFUNCTION(BlueprintImplementableEvent)
	void ChangeGunMaterial();
	UFUNCTION(BlueprintImplementableEvent)
	void IsShootingChange();
	
	UFUNCTION(BlueprintImplementableEvent)
	void ShootParticleEffect(FVector Source,FVector Target,FColor Color);
	UFUNCTION()
	void ChangeEnergyTextColor();
	UFUNCTION(BlueprintCallable,Category = "Energy Levels" )
	float GetEnergyLevel() { return EnergyLevel; }
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool isInHealingMode = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	FColor colorDamage = FColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	FColor colorHealing = FColor::Green;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	float EnergyLevel = 1000.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess))
	float MaxEnergyLevel = 1000.0f;
	UPROPERTY(BlueprintReadOnly)
	bool isShooting = false;
	UPROPERTY(BlueprintReadOnly)
	bool isGrabbing = false;

private:
	void TryToCatchOrb(USceneComponent *handSocket,float DeltaTime);
	void ShootLaser(USceneComponent *handSocket,float DeltaTime);
	void WalkByCatchOrb();
	
};
