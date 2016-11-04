// Fill out your copyright notice in the Description page of Project Settings.

#include "Octogonia.h"
#include "OctoPlayerController.h"

void AOctoPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("BeginPlay"));
}

void AOctoPlayerController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	// TODO: Do a lot of test to see the values accuracy and if the id is the right one

	FVector outPosition;
	FRotator outOrientation;

	bool foundLeftHand = USteamVRFunctionLibrary::GetHandPositionAndOrientation(0, EControllerHand::Left, outPosition, outOrientation);

	if (foundLeftHand)
	{
		character->ChangeLeftHandPosition(outPosition);
		character->ChangeLeftHandOrientation(outOrientation);
	}

	bool foundRightHand = USteamVRFunctionLibrary::GetHandPositionAndOrientation(0, EControllerHand::Right, outPosition, outOrientation);

	if (foundRightHand)
	{
		character->ChangeRightHandPosition(outPosition);
		character->ChangeRightHandOrientation(outOrientation);
	}
}

void AOctoPlayerController::SetPawn(APawn *pawn)
{
	Super::SetPawn(pawn);

	if (pawn && pawn->IsA(APlayerCharacter::StaticClass()))
	{
		character = Cast<APlayerCharacter>(pawn);
		BindPlayerInputs();
	}
	else
	{
		character = nullptr;
		InputComponent = nullptr;

		UE_LOG(LogTemp, Warning, TEXT("SetPawn"));
	}

}

void AOctoPlayerController::BindPlayerInputs()
{
	BindGameplayInput();

	// We start in the gameplay mode
	InputComponent = GameplayInput;
}

void AOctoPlayerController::BindGameplayInput()
{
	GameplayInput = NewObject<UInputComponent>(this, UInputComponent::StaticClass());

	GameplayInput->BindAction("Jump", IE_Pressed, this, &AOctoPlayerController::SendJumpCommand);
	GameplayInput->BindAction("TriggerLeftHand", IE_Pressed, this, &AOctoPlayerController::SendTriggerLeftHandCommand);
	GameplayInput->BindAction("TriggerLeftHand", IE_Released, this, &AOctoPlayerController::SendTriggerLeftHandCommand);
	GameplayInput->BindAction("TriggerRightHand", IE_Pressed, this, &AOctoPlayerController::SendTriggerRightHandCommandPressed);
	GameplayInput->BindAction("TriggerRightHand", IE_Released, this, &AOctoPlayerController::SendTriggerRightHandCommandReleased);
	GameplayInput->BindAction("TriggerRightGrip1", IE_Released, this, &AOctoPlayerController::SendGrip1RightHandCommand);
	GameplayInput->BindAxis("LookRight", this, &AOctoPlayerController::SendLookRightCommand);
	GameplayInput->BindAxis("LookUp", this, &AOctoPlayerController::SendLookUpCommand);
	GameplayInput->BindAxis("WalkForward", this, &AOctoPlayerController::SendWalkForwardCommand);
	GameplayInput->BindAxis("StrafeRight", this, &AOctoPlayerController::SendStrafeRightCommand);
}

void AOctoPlayerController::SendJumpCommand()
{
	character->Jump();
}

void AOctoPlayerController::SendLookRightCommand(float axisValue)
{
	character->LookRight(axisValue);

}

void AOctoPlayerController::SendLookUpCommand(float axisValue)
{
	character->LookUp(axisValue);
}

void AOctoPlayerController::SendWalkForwardCommand(float axisValue)
{
	character->WalkForward(axisValue);
}

void AOctoPlayerController::SendStrafeRightCommand(float axisValue)
{
	character->StrafeRight(axisValue);
}

void AOctoPlayerController::SendTriggerLeftHandCommand()
{
	character->TriggerLeftHand();
}
void AOctoPlayerController::SendTriggerRightHandCommandPressed()
{
	character->TriggerRightHandPressed();
}
void AOctoPlayerController::SendTriggerRightHandCommandReleased()
{
	character->TriggerRightHandReleased();
}
void AOctoPlayerController::SendGrip1RightHandCommand()
{
	character->ToogleShootingMode();
}

void AOctoPlayerController::OnDebugLeftHandYawValueChanged(float newValue)
{
	leftHandCurrentOrientation.Yaw = newValue;

	character->ChangeLeftHandOrientation(leftHandCurrentOrientation);
	UE_LOG(LogTemp, Warning, TEXT("New Left Hand Yaw Value: %f"), newValue);
}

void AOctoPlayerController::OnDebugLeftHandPitchValueChanged(float newValue)
{
	leftHandCurrentOrientation.Pitch = newValue;

	character->ChangeLeftHandOrientation(leftHandCurrentOrientation);
	UE_LOG(LogTemp, Warning, TEXT("New Left Hand Pitch Value: %f"), newValue);
}

void AOctoPlayerController::OnDebugRightHandYawValueChanged(float newValue)
{
	rightHandCurrentOrientation.Yaw = newValue;

	character->ChangeRightHandOrientation(rightHandCurrentOrientation);
	UE_LOG(LogTemp, Warning, TEXT("New Right Hand Yaw Value: %f"), newValue);
}
 
void AOctoPlayerController::OnDebugRightHandPitchValueChanged(float newValue)
{
	rightHandCurrentOrientation.Pitch = newValue;

	character->ChangeRightHandOrientation(rightHandCurrentOrientation);
	UE_LOG(LogTemp, Warning, TEXT("New Right Hand Pitch Value: %f"), newValue);
}