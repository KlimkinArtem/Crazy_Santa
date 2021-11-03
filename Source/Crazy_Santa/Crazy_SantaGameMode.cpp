// Copyright Epic Games, Inc. All Rights Reserved.

#include "Crazy_SantaGameMode.h"
#include "Crazy_SantaCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACrazy_SantaGameMode::ACrazy_SantaGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
