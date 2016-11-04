// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Runtime/Engine/Classes/Components/SplineMeshComponent.h"
#include "CustomSpline.generated.h"

UCLASS()
class OCTOGONIA_API ACustomSpline : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	USceneComponent *Root;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	USplineComponent *Spline;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	USceneComponent *MeshesContainer;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	UStaticMesh *MeshTemplate;

	/* If set to true, this property will add the missing mesh between the end and start spline points */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	bool BridgeStartAndEnd;

public:	

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnOrbs(FVector location);

	// Sets default values for this actor's properties
	ACustomSpline();


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void OnConstruction(const FTransform &Transform);

	const USplineComponent* GetSplineComponent() const;

private:
	void RenderSplinePoints();
	void AddMeshPart(const FVector &startLocation, const FVector &startTangent, const FVector &endLocation, const FVector &endTangent);
	
};
