// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "Bomb.generated.h"

UCLASS()
class CRAZY_SANTA_API ABomb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABomb();

	UPROPERTY(EditDefaultsOnly, Category= "RadialForce")
	TSubclassOf<AActor> RadialForce;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* Explosion;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundCue* BoomCue;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	FTimerHandle TimerHandle;

	void Damage();
};
