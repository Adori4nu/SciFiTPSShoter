// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiCharAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "TpsMultiCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SciFiTPSShoter/Weapon/Weapon.h"

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
	bWeaponEquipped = MultiCharacter->IsWeaponEquipped();
	EquippedWeapon = MultiCharacter->GetEquippedWeapon();
	bIsCrouched = MultiCharacter->bIsCrouched;
	bIsAiming = MultiCharacter->IsAiming();
	TurningInPlace = MultiCharacter->GetTurningInPlace();

	// Offset Yaw for Strafing
	FRotator AimRotation =  MultiCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(MultiCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 10.f);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = MultiCharacter->GetActorRotation();
	const FRotator DeltaCharacterRotation = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = DeltaCharacterRotation.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 10.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	AO_Yaw = MultiCharacter->GetAO_Yaw();
	AO_Pitch = MultiCharacter->GetAO_Pitch();

	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && MultiCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FRotator InRotation = FRotator(LeftHandTransform.GetRotation());
		FVector OutPosition;
		FRotator OutRotation;																		/*FRotator::ZeroRotator*/
		MultiCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), InRotation, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));
	}
}