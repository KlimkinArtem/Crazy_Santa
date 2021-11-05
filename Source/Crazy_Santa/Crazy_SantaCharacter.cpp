#include "Player/Santa.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/TransformCalculus3D.h"
#include "Sound/SoundCue.h"


ASanta::ASanta()
{

	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; 
}

void ASanta::BeginPlay()
{
	Super::BeginPlay();

	SpawnPick();
}



void ASanta::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ASanta::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASanta::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASanta::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASanta::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASanta::LookUpAtRate);

	PlayerInputComponent->BindAction("Debug", IE_Pressed, this, &ASanta::Debug);
	
	PlayerInputComponent->BindAction("Pick", IE_Pressed, this, &ASanta::SpawnPick);
	PlayerInputComponent->BindAction("Bomb", IE_Pressed, this, &ASanta::SpawnBomb);
	
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ASanta::Attack);
}

void ASanta::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASanta::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ASanta::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ASanta::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}



void ASanta::Debug()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Debug")));
}


void ASanta::SpawnPick()
{
	if(!GEngine) return;
	
	if(WeaponType == PICK || !bResetAnimMontage)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Pick Is valid")));
		return;
	}else if(WeaponType == BOMB)
	{
		Bomb->Destroy();
	}

	
	WeaponType = PICK;
	
	FString SocketName = "PickSocket";
	FVector Location = GetMesh()->GetSocketLocation(*SocketName);
	FRotator Rotation = GetMesh()->GetSocketRotation(*SocketName);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Socket loc = %s"), *Location.ToString()));
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Socket rot = %s"), *Rotation.ToString()));
	
	
	FActorSpawnParameters SpawnInfo;
	FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	
	Pick = GetWorld()->SpawnActor<AActor>(Weapons[0],Location, Rotation, SpawnInfo);
	
	Pick->AttachToComponent(GetMesh(), AttachRules, *SocketName);
}

void ASanta::SpawnBomb()
{
	if(Bombs <= 0 || !bResetAnimMontage || WeaponType == BOMB)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Bomb Is valid")));
		return;
	}
	
	Pick->Destroy();
	

    	
	

	WeaponType = BOMB;
	
	FString SocketName = "BombSocket";
	FVector Location = GetMesh()->GetSocketLocation(*SocketName);
	FRotator Rotation = GetMesh()->GetSocketRotation(*SocketName);

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Socket loc = %s"), *Location.ToString()));
	

	FActorSpawnParameters SpawnInfo;
	FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	
	Bomb = GetWorld()->SpawnActor<AActor>(Weapons[1],Location, Rotation, SpawnInfo);
	
	Bomb->AttachToComponent(GetMesh(), AttachRules, *SocketName);
}

void ASanta::Attack()
{
	switch (WeaponType)
	{
		case PICK:
			//CallFunctionByNameWithArguments(TEXT("PickAttackAnim"), ar, NULL, true);
			if(bResetAnimMontage)
			{
				PlayAnimMontage(AnimMontage);
				bResetAnimMontage = false;
			}
			
			break;
		case BOMB:
			BombAttack();
			break;
		default:
			break;
	}
}

void ASanta::PickAttack(float Radius, int32 Segments, bool bDrawDebugSphere)
{
	bPickCue = false;

	FString SocketName = "PickAttackSocket";
	FVector Location = GetMesh()->GetSocketLocation(*SocketName);
	FRotator Rotation = GetMesh()->GetSocketRotation(*SocketName);

	FActorSpawnParameters SpawnInfo;
	FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	
	auto Force = GetWorld()->SpawnActor<AActor>(RadialForce[0],Location, Rotation, SpawnInfo);
	Force->Destroy();

	FVector Start = GetMesh()->GetSocketLocation(TEXT("PickAttackSocket"));

	if(bDrawDebugSphere) DrawDebugSphere(GetWorld(), Start, Radius, Segments, FColor(181,0,0), true, 2, 0, 2);
	
	
	TArray<FHitResult> OutHits;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);
	
	bool bIsHit = GetWorld()->SweepMultiByChannel(OutHits, Start, Start, FQuat::Identity, ECC_Pawn, Sphere, Params);
	
	if (bIsHit)
	{
		
		for (auto& Hit : OutHits)
		{
			if (GEngine) 
			{
				if(Hit.Actor->ActorHasTag("Pick"))
				{
					if(bPickCue) return;
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("PICKkkk")));
					bPickCue = true;
					UGameplayStatics::SpawnSound2D(this, PickCue[0], 1);
					return;
				}
			}						
		}
	}
	
	UGameplayStatics::SpawnSound2D(this, PickCue[1], 1);
}

void ASanta::BombAttack()
{
	if(!bResetAnimMontage) return;
	
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("BombAttack")));

	FString SocketName = "BombSocket";
	FVector Location = GetMesh()->GetSocketLocation(*SocketName);
	FRotator Rotation = GetMesh()->GetSocketRotation(*SocketName);
	
	FActorSpawnParameters SpawnInfo;
	
	GetWorld()->SpawnActor<AActor>(PH_Bomb, Location, Rotation, SpawnInfo);

	Bombs -= 1;

	if(Bombs <= 0) SpawnPick();

}

