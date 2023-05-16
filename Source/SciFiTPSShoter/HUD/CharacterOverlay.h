// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class SCIFITPSSHOTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* HealthText;
};
