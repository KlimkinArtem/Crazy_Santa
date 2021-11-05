// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Misc/OutputDeviceNull.h"
#include "Santa.generated.h"

class USoundCue;
DECLARE_DELEGATE_TwoParams(FWeaponDelegate, FString, int);

UENUM()
enum EWeaponType
{
	NONE,
	PICK,
	BOMB,
};

UCLASS()
class CRAZY_SANTA_API ASanta : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
public:

	ASanta();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, Category= "Weapons")
	TArray<TSubclassOf<AActor>> Weapons;

	UPROPERTY(EditDefaultsOnly, Category= "Weapons")
	TSubclassOf<AActor> PH_Bomb;

	UPROPERTY(EditDefaultsOnly, Category= "RadialForce")
	TArray<TSubclassOf<AActor>> RadialForce;
	
	UFUNCTION(BlueprintCallable)
	void PickAttack(float Radius, int32 Segments, bool bDrawDebugSphere = false);
	
	UFUNCTION(BlueprintCallable)
	void BombAttack();

	UPROPERTY(EditAnywhere, Category = "Audio")
	TArray<USoundCue*> PickCue;

	UPROPERTY(BlueprintReadOnly)
	float Bombs = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Aniamtion")
	UAnimMontage* AnimMontage;

	UPROPERTY(BlueprintReadWrite)
	bool bResetAnimMontage = true;
	

private:

	void Debug();

	void SpawnPick();
	void SpawnBomb();
	
	void Attack();

	


	AActor* Pick;
	AActor* Bomb;
	FOutputDeviceNull ar;


	EWeaponType WeaponType = EWeaponType::NONE;

	bool bPickCue = false;
};





