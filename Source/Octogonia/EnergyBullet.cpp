// Fill out your copyright notice in the Description page of Project Settings.

#include "Octogonia.h"
#include "EnergyBullet.h"


// Sets default values
AEnergyBullet::AEnergyBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnergyBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnergyBullet::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

