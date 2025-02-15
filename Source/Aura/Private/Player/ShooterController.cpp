// Copyright Druid Mechanics


#include "Player/ShooterController.h"

#include "CharacterOverlay.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Aura/Components/CombatComponent.h"
#include "Character/ShooterCharacter.h"
#include "Components/TextBlock.h"
#include "UI/HUD/ShooterHud.h"

AShooterController::AShooterController()
{
	bReplicates = true;
}

void AShooterController::SetHUDWeaponAmmo(int32 Ammo)
{
	ShooterHud = ShooterHud == nullptr ? Cast<AShooterHud>(GetHUD()) : ShooterHud;
	bool bHUDValid = ShooterHud &&
		ShooterHud->CharacterOverlay &&
		ShooterHud->CharacterOverlay->AmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		ShooterHud->CharacterOverlay->AmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void AShooterController::SetHUDCarriedAmmo(int32 Ammo)
{
	ShooterHud = ShooterHud == nullptr ? Cast<AShooterHud>(GetHUD()) : ShooterHud;
	bool bHUDValid = ShooterHud &&
		ShooterHud->CharacterOverlay &&
		ShooterHud->CharacterOverlay->CarriedAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		ShooterHud->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}


void AShooterController::BeginPlay()
{
	Super::BeginPlay();
	check(ShooterContext);
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);
	Subsystem->AddMappingContext(ShooterContext, 0);
	DefaultMouseCursor = EMouseCursor::Default;
	FInputModeGameOnly  InputModeData;
	SetInputMode(InputModeData);
}

void AShooterController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveFowardAction, ETriggerEvent::Triggered, this, &AShooterController::MoveForward);
	EnhancedInputComponent->BindAction(MoveRightAction, ETriggerEvent::Triggered, this, &AShooterController::MoveRight);
	EnhancedInputComponent->BindAction(LookUpAction, ETriggerEvent::Triggered, this, &AShooterController::LookUp);
	EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &AShooterController::Turn);
	EnhancedInputComponent->BindAction(PickupAction, ETriggerEvent::Triggered, this, &AShooterController::Pickup);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AShooterController::Aim);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AShooterController::StopAim);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AShooterController::Jump);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AShooterController::Fire);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AShooterController::StopFire);
	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AShooterController::Reload);
	EnhancedInputComponent->BindAction(Weapon1, ETriggerEvent::Triggered, this, &AShooterController::Weapon1Action);
	EnhancedInputComponent->BindAction(Weapon2, ETriggerEvent::Triggered, this, &AShooterController::Weapon2Action);
	EnhancedInputComponent->BindAction(Weapon3, ETriggerEvent::Triggered, this, &AShooterController::Weapon3Action);
	EnhancedInputComponent->BindAction(Weapon4, ETriggerEvent::Triggered, this, &AShooterController::Weapon4Action);

}

void AShooterController::MoveForward(const FInputActionValue& InputActionValue)
{
	const float InputValue = InputActionValue.Get<float>();

	// Create a rotation that only uses the controller's yaw.
	// This ensures movement is restricted to the horizontal plane.
	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
            
	// Compute the forward direction from the yaw rotation
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
            
	GetPawn()->AddMovementInput(Direction, InputValue);
}

void AShooterController::MoveRight(const FInputActionValue& InputActionValue)
{
	const float InputValue = InputActionValue.Get<float>();
	// Construct a rotation that only uses the controller's yaw (ignoring pitch and roll)
	const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
            
	// Compute the right direction based on the yaw rotation.
	// Using the Y axis gives the right vector relative to the camera's horizontal facing.
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
            
	// Add movement input in the right direction scaled by the input value
	GetPawn()->AddMovementInput(Direction, InputValue);
}

void AShooterController::LookUp(const FInputActionValue& InputActionValue)
{
	const float InputValue = InputActionValue.Get<float>();
	AddPitchInput(InputValue);
}

void AShooterController::Turn(const FInputActionValue& InputActionValue)
{
	const float InputValue = InputActionValue.Get<float>();
	AddYawInput(InputValue);
}

void AShooterController::Pickup(const FInputActionValue& InputActionValue)
{

	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetPawn());
	if (ShooterCharacter)
	{
		ShooterCharacter->EquipWeapon();
	}
}

void AShooterController::Aim(const FInputActionValue& InputActionValue)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetPawn());
	if (ShooterCharacter)
	{
		ShooterCharacter->Aim();
	}
}

void AShooterController::StopAim(const FInputActionValue& InputActionValue)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetPawn());
	if (ShooterCharacter)
	{
		ShooterCharacter->StopAim();
	}
}

void AShooterController::Jump(const FInputActionValue& InputActionValue)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetPawn());
	if (ShooterCharacter && ShooterCharacter->bCanJump)
	{
		ShooterCharacter->Jump();
	}
}

void AShooterController::Fire(const FInputActionValue& InputActionValue)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetPawn());
	if (ShooterCharacter && ShooterCharacter->bCanJump)
	{
		ShooterCharacter->Fire();
	}
}

void AShooterController::StopFire(const FInputActionValue& InputActionValue)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetPawn());
	if (ShooterCharacter && ShooterCharacter->bCanJump)
	{
		ShooterCharacter->StopFire();
	}
}

void AShooterController::Reload(const FInputActionValue& InputActionValue)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetPawn());
	if (ShooterCharacter && ShooterCharacter->bCanJump)
	{
		ShooterCharacter->Reload();
	}
}

void AShooterController::Weapon1Action(const FInputActionValue& InputActionValue)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetPawn());
	if (ShooterCharacter)
	{
		AWeapon* PistolWeapon = ShooterCharacter->Combat->PistolWeapon;
		if (PistolWeapon == nullptr) return;
		ShooterCharacter->Combat->EquipWeapon(PistolWeapon);
	}
}

void AShooterController::Weapon2Action(const FInputActionValue& InputActionValue)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetPawn());
	if (ShooterCharacter)
	{
		AWeapon* ShotgunWeaopn = ShooterCharacter->Combat->ShotgunWeaopn;
		if (ShotgunWeaopn == nullptr) return;
		ShooterCharacter->Combat->EquipWeapon(ShotgunWeaopn);
	}
}

void AShooterController::Weapon3Action(const FInputActionValue& InputActionValue)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetPawn());
	if (ShooterCharacter)
	{
		AWeapon* AssaultRifleWeapon = ShooterCharacter->Combat->AssaultRifleWeapon;
		if (AssaultRifleWeapon == nullptr) return;
		ShooterCharacter->Combat->EquipWeapon(AssaultRifleWeapon);
	}
}

void AShooterController::Weapon4Action(const FInputActionValue& InputActionValue)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetPawn());
	if (ShooterCharacter)
	{
		AWeapon* RocketLauncherWeapon = ShooterCharacter->Combat->RocketLauncherWeapon;
		if (RocketLauncherWeapon == nullptr) return;
		ShooterCharacter->Combat->EquipWeapon(RocketLauncherWeapon);
	}
}
