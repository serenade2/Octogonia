// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "PlayerCharacter.h"
#include "OctoPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class OCTOGONIA_API AOctoPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// FOR DEBUG ONLY
	UFUNCTION(BlueprintCallable, Category = "VR Debug")
	void OnDebugLeftHandYawValueChanged(float newValue);

	// FOR DEBUG ONLY
	UFUNCTION(BlueprintCallable, Category = "VR Debug")
	void OnDebugLeftHandPitchValueChanged(float newValue);

	// FOR DEBUG ONLY
	UFUNCTION(BlueprintCallable, Category = "VR Debug")
	void OnDebugRightHandYawValueChanged(float newValue);

	// FOR DEBUG ONLY
	UFUNCTION(BlueprintCallable, Category = "VR Debug")
	void OnDebugRightHandPitchValueChanged(float newValue);

	void BeginPlay() override;
	void Tick(float deltaTime) override;

	void SetPawn(APawn *pawn) override;
	
private:
	APlayerCharacter *character;
	UInputComponent *GameplayInput;

	void BindPlayerInputs();
	void BindGameplayInput();

	void SendJumpCommand();
	void SendLookRightCommand(float axisValue);
	void SendLookUpCommand(float axisValue);
	void SendWalkForwardCommand(float axisValue);
	void SendStrafeRightCommand(float axisValue);
	void SendTriggerLeftHandCommand();
	void SendTriggerRightHandCommandPressed();
	void SendTriggerRightHandCommandReleased();
	void SendGrip1RightHandCommand();

	// FOR DEBUG ONLY
	FRotator rightHandCurrentOrientation;
	FVector rightHandCurrentPosition;
	FRotator leftHandCurrentOrientation;
	FVector leftHandCurrentPosition;
};
