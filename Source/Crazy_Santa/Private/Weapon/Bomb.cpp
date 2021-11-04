// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Bomb.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Math/TransformCalculus3D.h"


ABomb::ABomb()
{
	PrimaryActorTick.bCanEverTick = false;
}


void ABomb::BeginPlay()
{
	Super::BeginPlay();


	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
	{
		FActorSpawnParameters SpawnInfo;
		GetWorld()->SpawnActor<AActor>(RadialForce, GetActorLocation(), GetActorRotation(), SpawnInfo);
		Damage();
		
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Explosion, GetActorLocation(), GetActorRotation(), FVector(3));
		UGameplayStatics::SpawnSound2D(this, BoomCue, 1);
		
		Destroy();
	}, 5, false);
	
}

void ABomb::Damage()
{
	TArray<FHitResult> OutHits;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	FCollisionShape Sphere = FCollisionShape::MakeSphere(300);
	
	bool bIsHit = GetWorld()->SweepMultiByChannel(OutHits, GetActorLocation(), GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere, Params);
	
	if (bIsHit)
	{
		
		for (auto& Hit : OutHits)
		{
			if (GEngine) 
			{
				if(Hit.Actor->ActorHasTag("Player"))
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("PLAYER!!!")));
				}
			}						
		}
	}
}
