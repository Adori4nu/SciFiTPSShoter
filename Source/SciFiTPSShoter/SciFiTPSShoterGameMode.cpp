// Copyright Epic Games, Inc. All Rights Reserved.

#include "SciFiTPSShoterGameMode.h"
#include "SciFiTPSShoterCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASciFiTPSShoterGameMode::ASciFiTPSShoterGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
