// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayerController.h"
#include "SciFiTPSShoter/HUD/TPSCharacterHUD.h"
#include "SciFiTPSShoter/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


void ATPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TPSHUD = Cast<ATPSCharacterHUD>(GetHUD());
}

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
		FString HealthText = FString::Printf(TEXT("%d"), HealthPercent);
		TPSHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}