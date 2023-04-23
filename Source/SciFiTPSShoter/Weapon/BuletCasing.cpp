// Fill out your copyright notice in the Description page of Project Settings.


#include "BuletCasing.h"

// Sets default values
ABuletCasing::ABuletCasing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);
}

// Called when the game starts or when spawned
void ABuletCasing::BeginPlay()
{
	Super::BeginPlay();
	
}

