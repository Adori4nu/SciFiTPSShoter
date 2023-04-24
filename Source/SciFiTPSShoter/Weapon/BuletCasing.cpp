// Fill out your copyright notice in the Description page of Project Settings.


#include "BuletCasing.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
ABuletCasing::ABuletCasing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CasingMesh->SetSimulatePhysics(true);
	CasingMesh->SetEnableGravity(true);
	CasingMesh->SetNotifyRigidBodyCollision(true);
	ShellEjectionImpulseValue = 10.f;

	MinCoolTime = 1.f;
	MaxCoolTime = 5.f;
	CoolTime = FMath::RandRange(MinCoolTime, MaxCoolTime);
	CoolRate = 0.5f;
}

// Called when the game starts or when spawned
void ABuletCasing::BeginPlay()
{
	Super::BeginPlay();
	
	CasingMesh->AddImpulse(GetActorForwardVector() * ShellEjectionImpulseValue);
	CasingMesh->OnComponentHit.AddDynamic(this, &ABuletCasing::OnHit);

	DynamicMaterialInstance = CasingMesh->CreateDynamicMaterialInstance(0);
	if (DynamicMaterialInstance)
	{
		GetWorldTimerManager().SetTimer(CoolTimerHandle, this, &ABuletCasing::Cool, CoolRate, true);

		EmissivePower = DynamicMaterialInstance->K2_GetScalarParameterValue(FName("Emissive_Strength"));
		EmissivePowerDelta = (EmissivePower / CoolTime) * CoolRate;
	}
}

void ABuletCasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, [this] { Destroy(); }, 5.f, false);
	
	CasingMesh->SetNotifyRigidBodyCollision(false);
}

void ABuletCasing::Cool()
{
	if (DynamicMaterialInstance == nullptr) { return; }

	EmissivePower -= EmissivePowerDelta;
	DynamicMaterialInstance->SetScalarParameterValue(FName("Emissive_Strength"), EmissivePower);

	if (EmissivePower <= 0.f)
	{
		GetWorldTimerManager().ClearTimer(CoolTimerHandle);
	}
}

