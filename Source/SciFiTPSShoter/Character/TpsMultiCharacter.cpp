// Fill out your copyright notice in the Description page of Project Settings.


#include "TpsMultiCharacter.h"
#include "Camera/CameraComponent.h"
#include "CharacterComponents/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/WidgetComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "SciFiTPSShoter/GameMode/TPSCoOpGameMode.h"
#include "SciFiTPSShoter/PlayerController/TPSPlayerController.h"
#include "SciFiTPSShoter/SciFiTPSShoter.h"
#include "SciFiTPSShoter/Weapon/Weapon.h"
#include "MultiCharAnimInstance.h"


// Sets default values
ATpsMultiCharacter::ATpsMultiCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 250.0f;
	CameraBoom->SocketOffset = FVector(100.f, 70.f, 50.f);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->ProbeSize = 12.f;
	CameraBoom->ProbeChannel = ECC_Camera;
	CameraBoom->bDoCollisionTest = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComp->SetIsReplicated(true);

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	NetUpdateFrequency = 120.f;
	MinNetUpdateFrequency = 60.f;
}

void ATpsMultiCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ATpsMultiCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ATpsMultiCharacter, Health);
}

void ATpsMultiCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (CombatComp)
	{
		CombatComp->Character = this;
	}
}

void ATpsMultiCharacter::PlayFireMontage(bool bAiming)
{
	if (CombatComp == nullptr || CombatComp->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ATpsMultiCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

void ATpsMultiCharacter::PlayHitReactMontage()
{
	if (CombatComp == nullptr || CombatComp->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage && !bEliminated)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ATpsMultiCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UpdateHUDHealth();
	PlayHitReactMontage();

	if (Health == 0.f)
	{
		ATPSCoOpGameMode* CoOpGameMode = GetWorld()->GetAuthGameMode<ATPSCoOpGameMode>();
		if (CoOpGameMode)
		{
			TPSPlayerController = TPSPlayerController == nullptr ? Cast<ATPSPlayerController>(Controller) : TPSPlayerController;
			ATPSPlayerController* AttackerController = Cast<ATPSPlayerController>(InstigatorController);
			CoOpGameMode->PlayerEliminated(this, TPSPlayerController, AttackerController);
		}
	}
}

void ATpsMultiCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();

	if (GetLocalRole() == ENetRole::ROLE_SimulatedProxy)
	{
		SimProxiesTurn();
	}
	TimeSinceLastMovementReplication = 0.f;
}

void ATpsMultiCharacter::Elim()
{
	MulticastElim();
}

void ATpsMultiCharacter::MulticastElim_Implementation()
{
	bEliminated = true;
	PlayElimMontage();
}

void ATpsMultiCharacter::MulticastHit_Implementation()
{
	PlayHitReactMontage();
}

// Called when the game starts or when spawned
void ATpsMultiCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateHUDHealth();
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ATpsMultiCharacter::ReceiveDamage);
	}
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::
			GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(TpsCharacterContext, 0);
		}
	}
}

// Called every frame
void ATpsMultiCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);

	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.125f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}

	HideCameraIfCharacterClose();
}

// Called to bind functionality to input
void ATpsMultiCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATpsMultiCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATpsMultiCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ATpsMultiCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Triggered, this, &ATpsMultiCharacter::Interact);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ATpsMultiCharacter::Crouch);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ATpsMultiCharacter::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ATpsMultiCharacter::StopAim);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ATpsMultiCharacter::Fire);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &ATpsMultiCharacter::StopFiring);
	}
}

void ATpsMultiCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ATpsMultiCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ATpsMultiCharacter::Aim()
{
	if (CombatComp)
	{
		CombatComp->SetAiming(true);
	}
}

void ATpsMultiCharacter::StopAim()
{
	if (CombatComp)
	{
		CombatComp->SetAiming(false);
	}
}

void ATpsMultiCharacter::Interact()
{
	if (CombatComp)
	{
		if (HasAuthority())
		{
			CombatComp->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}
	}
}

void ATpsMultiCharacter::Crouch()
{
	if (bIsCrouched)
	{
		Super::UnCrouch();
	}
	else
	{
		Super::Crouch();
	}
}

void ATpsMultiCharacter::Jump()
{
	if (bIsCrouched)
	{
		Super::UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void ATpsMultiCharacter::Fire()
{
	if (CombatComp)
	{
		CombatComp->FireButtonPressed(true);
	}
}

void ATpsMultiCharacter::StopFiring()
{
	if (CombatComp)
	{
		CombatComp->FireButtonPressed(false);
	}
}

void ATpsMultiCharacter::SimProxiesTurn()
{
	if (CombatComp == nullptr || CombatComp->EquippedWeapon == nullptr) return;
	
	bRotateRootBone = false;

	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame);

	if (FMath::Abs(ProxyYaw) > TurnTreshold)
	{
		if (ProxyYaw > TurnTreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnTreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

float ATpsMultiCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

void ATpsMultiCharacter::AimOffset(float DeltaTime)
{
	if (CombatComp && CombatComp->EquippedWeapon == nullptr) return;
	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir) // standing still not jumping
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) // running, or jumping
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	CalculateAO_Pitch();
}

void ATpsMultiCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;

	if (AO_Pitch && !IsLocallyControlled())
	{
		// map pitch from [270, 360) to [-90 , 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ATpsMultiCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 2.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void ATpsMultiCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}

}

void ATpsMultiCharacter::ServerEquipButtonPressed_Implementation()
{
	if (CombatComp)
	{
		CombatComp->EquipWeapon(OverlappingWeapon);
	}
}

void ATpsMultiCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraTreshold)
	{
		GetMesh()->SetVisibility(false);
		if (CombatComp && CombatComp->EquippedWeapon && CombatComp->EquippedWeapon->GetWeaponMesh())
		{
			CombatComp->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (CombatComp && CombatComp->EquippedWeapon && CombatComp->EquippedWeapon->GetWeaponMesh())
		{
			CombatComp->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void ATpsMultiCharacter::UpdateHUDHealth()
{
	TPSPlayerController = TPSPlayerController == nullptr ? Cast<ATPSPlayerController>(Controller) : TPSPlayerController;
	if (TPSPlayerController)
	{
		TPSPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void ATpsMultiCharacter::OnRep_Health()
{
	UpdateHUDHealth();
	PlayHitReactMontage();
}

void ATpsMultiCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool ATpsMultiCharacter::IsWeaponEquipped()
{
	return (CombatComp && CombatComp->EquippedWeapon);
}

bool ATpsMultiCharacter::IsAiming()
{
	return (CombatComp && CombatComp->bAiming);
}

AWeapon* ATpsMultiCharacter::GetEquippedWeapon()
{
	if (CombatComp == nullptr) return nullptr;
	return CombatComp->EquippedWeapon;
}

FVector ATpsMultiCharacter::GetHitTarget() const
{
	if (CombatComp == nullptr) return FVector();
	return CombatComp->HitTarget;
}

