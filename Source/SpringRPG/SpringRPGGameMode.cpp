// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpringRPGGameMode.h"
#include "SpringRPGCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASpringRPGGameMode::ASpringRPGGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
