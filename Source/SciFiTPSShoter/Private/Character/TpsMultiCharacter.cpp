// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TpsMultiCharacter.h"

// Sets default values
ATpsMultiCharacter::ATpsMultiCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATpsMultiCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATpsMultiCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATpsMultiCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

