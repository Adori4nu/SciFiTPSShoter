// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerController.h"
#include "SciFiTPSShoter/Character/TpsMultiCharacter.h"
#include "SciFiTPSShoter/HUD/TPSCharacterHUD.h"
#include "SciFiTPSShoter/HUD/CharacterOverlay.h"

void ATPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TPSHUD = Cast<ATPSCharacterHUD>(GetHUD());
}

//void ATPSPlayerController::OnPossess(APawn* InPawn)
//{
//	Super::OnPossess(InPawn);
//	
//	if (ATpsMultiCharacter* MultiChar = Cast<ATpsMultiCharacter>(InPawn); MultiChar)
//	{
//		SetHUDHealth(MultiChar->GetHealth(), MultiChar->GetMaxHealth());
//	}
//}

void ATPSPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	TPSHUD = TPSHUD == nullptr ? Cast<ATPSCharacterHUD>(GetHUD()) : TPSHUD;
	bool bHUDValid = TPSHUD &&
		TPSHUD->CharacterOverlay &&
		TPSHUD->CharacterOverlay->HealthBar &&
		TPSHUD->CharacterOverlay->HealthText;
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		TPSHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		TPSHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}