#include "Player/Santa.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


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
	if(IsValid(Pick)) return;
	if(IsValid(Bomb)) Bomb->Destroy();

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
	if(IsValid(Bomb) || Bombs <= 0) return;
	if(IsValid(Pick)) Pick->Destroy();

	WeaponType = BOMB;
	
	FString SocketName = "BombWeapon";
	FVector Location = GetMesh()->GetSocketLocation(*SocketName);
	FRotator Rotation = GetMesh()->GetSocketRotation(*SocketName);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Socket loc = %s"), *Location.ToString()));
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Socket rot = %s"), *Rotation.ToString()));

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
			PickAttack();
			break;
		case BOMB:
			BombAttack();
			break;
		default:
			break;
	}
}

void ASanta::PickAttack()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("PickAttack")));
}

void ASanta::BombAttack()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("BombAttack")));

}

