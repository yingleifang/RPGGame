// Copyright Druid Mechanics


#include "CombatComponent.h"
#include "Aura/Weapon/Weapon.h"
#include "Aura/Public/Character/ShooterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Aura/Public/Player/ShooterController.h"
#include "Aura/Public/UI/HUD/ShooterHUD.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	if (EquippedWeaponClass)
	{
		EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(EquippedWeaponClass);
	}
	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 400.f;
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(true);
	if (TargetCharacter)
	{
		TargetCharacter->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}
	EquipWeapon(EquippedWeapon);
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SetHudCrosshairs(DeltaTime);
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	TargetCharacter->StartingAimRotation = FRotator(0.f, TargetCharacter->GetBaseAimRotation().Yaw, 0.f);

	if (TargetCharacter == nullptr || WeaponToEquip == nullptr) return;
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = TargetCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, TargetCharacter->GetMesh());
	}
	EquippedWeapon->SetOwner(TargetCharacter);
	EquippedWeapon->ShowPickupWidget(false);
	TargetCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
	TargetCharacter->bUseControllerRotationYaw = true;
	
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;
	if (TargetCharacter)
	{
		TargetCharacter->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::SetFiring(bool bPressed)
{
	bFiring = bPressed;
	if (EquippedWeapon == nullptr) return;
	if (TargetCharacter && bPressed)
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		TargetCharacter->PlayFireMontage(bPressed);
		EquippedWeapon->Fire(HitResult.ImpactPoint);
	}
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);
	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);
	}
}

void UCombatComponent::SetHudCrosshairs(float DeltaTime)
{
	if (TargetCharacter == nullptr || TargetCharacter->Controller == nullptr) return;
	TargetController = TargetController == nullptr ? Cast<AShooterController>(TargetCharacter->Controller) : TargetController;
	if (TargetController)
	{
		TargetHud = TargetHud == nullptr ? Cast<AShooterHud>(TargetController->GetHUD()) : TargetHud;
		if (TargetHud)
		{
			FHUDPackage HUDPackage;
			if (EquippedWeapon)
			{
				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
			}
			FVector2D WalkSpeedRange(0.f, TargetCharacter->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = TargetCharacter->GetVelocity();
			Velocity.Z = 0.f;
			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());
			if (TargetCharacter->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}
			HUDPackage.CrosshairSpread = CrosshairVelocityFactor + CrosshairInAirFactor;
			TargetHud->SetHUDPackage(HUDPackage);
		}
	}
}
