// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "SciFiTPSShoter/PlayerController/TPSPlayerController.h"
#include "SciFiTPSShoter/Character/TpsMultiCharacter.h"
#include "GameFramework/Character.h"

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//UE_LOG(LogTemp, Warning, TEXT("ProjectileBullet OnHit Start!"));
	ATpsMultiCharacter* OwnerCharacter = Cast<ATpsMultiCharacter>(GetOwner());
	if (OwnerCharacter)
	{
		ATPSPlayerController* OwnerController = Cast<ATPSPlayerController>(OwnerCharacter->Controller);
		//OtherActor
		if (OwnerController)
		{//GetInstigator()->Controller
			ATpsMultiCharacter* DamagedActor = Cast<ATpsMultiCharacter>(OtherActor);
			//Multicast_DealDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
			auto dmg = UGameplayStatics::ApplyDamage(DamagedActor, Damage, OwnerController, this, UDamageType::StaticClass());
			UE_LOG(LogTemp, Warning, TEXT("ApplyDamage %s dealed %f damage to %s target controller: %s"), *OwnerCharacter->GetName(), /*dmg,*/ *OtherActor->GetName(), *OwnerController->GetName());
			Super::OnHit( HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
			return;
		}
	}
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

//float AProjectileBullet::DealDamage(AActor* DamagedActor, float BaseDamage, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass)
//{
//	if (DamagedActor && (BaseDamage != 0.f))
//	{
//		// make sure we have a good damage type
//		TSubclassOf<UDamageType> const ValidDamageTypeClass = DamageTypeClass ? DamageTypeClass : TSubclassOf<UDamageType>(UDamageType::StaticClass());
//		FDamageEvent DamageEV(ValidDamageTypeClass);
//
//		return DamagedActor->TakeDamage(BaseDamage, DamageEV, EventInstigator, DamageCauser);
//	}
//
//	return 0.f;
//}
//
//void AProjectileBullet::Multicast_DealDamage_Implementation(AActor* DamagedActor, float BaseDamage, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass)
//{
//	DealDamage(DamagedActor, BaseDamage, EventInstigator, DamageCauser, DamageTypeClass);
//}
