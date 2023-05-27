// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GameState)
	{
		int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

		if (NumberOfPlayers == 1)
		{
			if (UWorld* World = GetWorld(); World)
			{
				bUseSeamlessTravel = true;
				World->ServerTravel(FString("/Game/MyContent/Maps/GamePlay_Map?listen"));
			}
		}
	}
}
