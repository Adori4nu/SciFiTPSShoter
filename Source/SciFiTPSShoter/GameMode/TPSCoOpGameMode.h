// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TPSCoOpGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SCIFITPSSHOTER_API ATPSCoOpGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	virtual void PlayerEliminated(class ATpsMultiCharacter* EliminatedCharacter,
		class ATPSPlayerController* VictimController,
		class ATPSPlayerController* AttackerController
		);
};
