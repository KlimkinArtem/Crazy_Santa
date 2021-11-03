// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Santa.h"

// Sets default values
ASanta::ASanta()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASanta::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASanta::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASanta::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Debug", IE_Pressed, this, &ASanta::Debug);
}

void ASanta::Debug()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Debug")));
}

