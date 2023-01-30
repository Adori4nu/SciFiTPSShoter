// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiCharAnimInstance.h"
#include "TpsMultiCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMultiCharAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	MultiCharacter = Cast<ATpsMultiCharacter>(TryGetPawnOwner());
}

void UMultiCharAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (MultiCharacter == nullptr)
	{
		MultiCharacter = Cast<ATpsMultiCharacter>(TryGetPawnOwner());
	}
	if (MultiCharacter == nullptr) return;

	FVector Velocity = MultiCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = MultiCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelerating = MultiCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
}