#include "Player/Santa.h"

#include "DrawDebugHelpers.h"
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
	
	FString SocketName = "BombSocket";
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
			CallFunctionByNameWithArguments(TEXT("PickAttackAnim"), ar, NULL, true);
			break;
		case BOMB:
			CallFunctionByNameWithArguments(TEXT("BombAttackEvent"), ar, NULL, true);
			break;
		default:
			break;
	}
}

void ASanta::PickAttack(float Radius, int32 Segments)
{



	FString SocketName = "PickAttackSocket";
	FVector Location = GetMesh()->GetSocketLocation(*SocketName);
	FRotator Rotation = GetMesh()->GetSocketRotation(*SocketName);

	FActorSpawnParameters SpawnInfo;
	FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	
	auto Force = GetWorld()->SpawnActor<AActor>(RadialForce[0],Location, Rotation, SpawnInfo);
	Force->Destroy();

	
	FVector Start = FollowCamera->GetComponentLocation();
	FVector FollowCameraForwardVector = FollowCamera->GetForwardVector();

	FHitResult OutHit;
	
	FVector End = ((FollowCameraForwardVector * 400.f) + Start);
	FCollisionQueryParams CollisionParams;

	CollisionParams.AddIgnoredActor(this);
	
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 3, 0, 2);

	bool bIsHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Pawn, CollisionParams);

	
	
	if(bIsHit)
	{
		if(GEngine)
		{
			if(OutHit.Actor->ActorHasTag("Pick"))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("PICKCKCKC")));
			}
		}

		
	}

	
}

void ASanta::BombAttack()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("BombAttack")));

}

