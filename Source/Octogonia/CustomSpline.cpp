// Fill out your copyright notice in the Description page of Project Settings.

#include "Octogonia.h"
#include "CustomSpline.h"

// Sets default values
ACustomSpline::ACustomSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
	Spline->SetupAttachment(RootComponent);

	MeshesContainer = CreateDefaultSubobject<USceneComponent>("RailMeshesContainer");
	MeshesContainer->SetupAttachment(RootComponent);

}

void ACustomSpline::OnConstruction(const FTransform &Transform)
{
	RenderSplinePoints();
}

// Called when the game starts or when spawned
void ACustomSpline::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACustomSpline::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ACustomSpline::RenderSplinePoints()
{
	if (Spline && MeshTemplate)
	{
		TArray<USceneComponent *> children;
		MeshesContainer->GetChildrenComponents(false, children);

		for (USceneComponent *child : children)
		{
			if (child)
			{
				child->DestroyComponent(false);
			}
		}

		
		int pointsCount = Spline->GetNumberOfSplinePoints();
		long fullDistance = Spline->GetDistanceAlongSplineAtSplinePoint(pointsCount-1);

		//UE_LOG(LogTemp, Warning, TEXT("%d"), pointsCount);

		FVector startLocation, startTangent, endLocation, endTangent;


		long SplineResolution = 500;

		long aDistance = 0;
		UE_LOG(LogTemp, Warning, TEXT("fullDistance = %f"), fullDistance);
		while (aDistance <= fullDistance) {

			startLocation = Spline->GetLocationAtDistanceAlongSpline(aDistance, ESplineCoordinateSpace::Local);
			startTangent = Spline->GetTangentAtDistanceAlongSpline(aDistance, ESplineCoordinateSpace::Local);

			aDistance += SplineResolution;

			endLocation = Spline->GetLocationAtDistanceAlongSpline(aDistance, ESplineCoordinateSpace::Local);
			endTangent = Spline->GetTangentAtDistanceAlongSpline(aDistance, ESplineCoordinateSpace::Local);

			AddMeshPart(startLocation, startTangent, endLocation, endTangent);
		}

		/*for (int i = 1; i < pointsCount; i++)
		{
			Spline->GetLocalLocationAndTangentAtSplinePoint(i - 1, startLocation, startTangent);
			Spline->GetLocalLocationAndTangentAtSplinePoint(i, endLocation, endTangent);

			AddMeshPart(startLocation, startTangent, endLocation, endTangent);
		}

		if (BridgeStartAndEnd)
		{
			Spline->GetLocalLocationAndTangentAtSplinePoint(0, startLocation, startTangent);
			Spline->GetLocalLocationAndTangentAtSplinePoint(pointsCount - 1, endLocation, endTangent);
			AddMeshPart(startLocation, startTangent, endLocation, endTangent);
		}*/
	}
}

void ACustomSpline::AddMeshPart(const FVector &startLocation, const FVector &startTangent, const FVector &endLocation, const FVector &endTangent)
{
	FVector offset = MeshesContainer->GetComponentLocation();

	USplineMeshComponent *railMesh = NewObject<USplineMeshComponent>(MeshesContainer);
	railMesh->SetStaticMesh(MeshTemplate);

	railMesh->SetForwardAxis(ESplineMeshAxis::Z);

	railMesh->SetStartAndEnd(startLocation + offset, startTangent, endLocation + offset, endTangent);

	railMesh->SetMobility(EComponentMobility::Movable);
	railMesh->SetupAttachment(MeshesContainer);
	railMesh->RegisterComponent();
}

const USplineComponent* ACustomSpline::GetSplineComponent() const
{
	return Spline;
}