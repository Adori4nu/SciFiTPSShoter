// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSCoOpGameMode.h"
#include "SciFiTPSShoter/Character/TpsMultiCharacter.h"
#include "SciFiTPSShoter/PlayerController/TPSPlayerController.h"

void ATPSCoOpGameMode::PlayerEliminated(ATpsMultiCharacter* EliminatedCharacter, ATPSPlayerController* VictimController, ATPSPlayerController* AttackerController)
{
	if (EliminatedCharacter)
	{
		EliminatedCharacter->Elim();
	}
}
