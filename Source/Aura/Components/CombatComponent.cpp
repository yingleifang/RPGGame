// Copyright Druid Mechanics


#include "CombatComponent.h"

#include "CharacterOverlay.h"
#include "Aura/Weapon/Weapon.h"
#include "Aura/Public/Character/ShooterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Aura/Public/Player/ShooterController.h"
#include "Aura/Public/UI/HUD/ShooterHUD.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Components/Image.h"
#include "Sound/SoundCue.h"


// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
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
		if (TargetCharacter->GetFollowCamera())
		{
			DefaultFOV = TargetCharacter->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}

	InitializeCarriedAmmo();
	EquipWeapon(GetWorld()->SpawnActor<AWeapon>(EquippedWeaponClass));
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (TargetCharacter && TargetCharacter->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;
		SetHudCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);

		CurrentRecoil = FMath::RInterpTo(CurrentRecoil, FRotator::ZeroRotator, DeltaTime, EquippedWeapon->RecoilRecoverySpeed);
		APlayerController* PC = Cast<APlayerController>(Cast<APawn>(GetOwner())->GetController());
		if (PC)
		{
			// Retrieve the current control rotation.
			FRotator ControlRotation = PC->GetControlRotation();
			
			PC->SetControlRotation(ControlRotation + CurrentRecoil);
		}
	}
}

TSubclassOf<UGameplayAbility> UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	WeaponToEquip->EnableCustomDepth(false);
	WeaponToEquip->GetWeaponMesh()->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));

	if (IsValid(EquippedWeapon))
	{
		EquippedWeapon->SetActorHiddenInGame(true);
	}
	
	switch (WeaponToEquip->GetWeaponType())
	{
	case EWeaponType::EWT_AssaultRifle:
		TargetHud->CharacterOverlay->AssaultRifleImage->SetVisibility(ESlateVisibility::Visible);
		AssaultRifleWeapon = WeaponToEquip;
		break;
	case EWeaponType::EWT_Shotgun:
		TargetHud->CharacterOverlay->ShugunImage->SetVisibility(ESlateVisibility::Visible);
		ShotgunWeaopn = WeaponToEquip;
		break;
	case EWeaponType::EWT_RocketLauncher:
		TargetHud->CharacterOverlay->RocketLauncherImage->SetVisibility(ESlateVisibility::Visible);
		RocketLauncherWeapon = WeaponToEquip;
		break;
	case EWeaponType::EWT_Pistol:
		if (PistolWeapon == nullptr)
			PistolWeapon = WeaponToEquip;
	}
	
	TargetCharacter->StartingAimRotation = FRotator(0.f, TargetCharacter->GetBaseAimRotation().Yaw, 0.f);

	if (TargetCharacter == nullptr || WeaponToEquip == nullptr) return nullptr;
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
	EquippedWeapon->SetHudAmmo();
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	TargetController = TargetController == nullptr ? Cast<AShooterController>(TargetCharacter->Controller) : TargetController;
	if (TargetController)
	{
		TargetController->SetHUDCarriedAmmo(CarriedAmmo);
	}
	if (EquippedWeapon->EquipSound)
	{
		UGameplayStatics::PlaySound2D(
			this,
			EquippedWeapon->EquipSound
		);
	}
	if (EquippedWeapon->IsEmpty())
	{
		Reload();
	}
	EquippedWeapon->SetActorHiddenInGame(false);
	return WeaponToEquip->WeaponAbilityClass;
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;
	if (TargetCharacter)
	{
		TargetCharacter->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

void UCombatComponent::Fire()
{
	if (CanFire() && CombatState == ECombatState::ECS_Unoccupied)
	{
		bCanFire = false;
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		TargetCharacter->PlayFireMontage();
		EquippedWeapon->Fire(HitResult.ImpactPoint);
		StartFireTimer();

		float RecoilPitch = FMath::RandRange(EquippedWeapon->RecoilPitchMin, EquippedWeapon->RecoilPitchMax);
		float RecoilYaw = FMath::RandRange(-EquippedWeapon->RecoilYawRange, EquippedWeapon->RecoilYawRange);

		// Add the recoil impulse to our current recoil offset
		CurrentRecoil.Pitch += RecoilPitch;
		CurrentRecoil.Yaw += RecoilYaw;
		APlayerController* PC = Cast<APlayerController>(Cast<APawn>(GetOwner())->GetController());
		if (PC)
			PC->ClientStartCameraShake(FireCameraShakeClass, EquippedWeapon->CameraShakeScale);
	}
	
}

void UCombatComponent::SetFiring(bool bPressed)
{
	bFiring = bPressed;
	if (EquippedWeapon == nullptr) return;
	if (TargetCharacter && bPressed)
	{
		Fire();
	}
}

void UCombatComponent::UpdateAmmoValues()
{
	if (TargetCharacter == nullptr || EquippedWeapon == nullptr) return;
	int32 ReloadAmount = AmountToReload();
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	TargetController = TargetController == nullptr ? Cast<AShooterController>(TargetCharacter->Controller) : TargetController;
	if (TargetController)
	{
		TargetController->SetHUDCarriedAmmo(CarriedAmmo);
	}
	EquippedWeapon->AddAmmo(ReloadAmount);
}

void UCombatComponent::StartFireTimer()
{
	if (EquippedWeapon == nullptr || TargetCharacter == nullptr) return;
	TargetCharacter->GetWorldTimerManager().SetTimer(
	FireTimer,
	this,
	&UCombatComponent::FireTimerFinished,
	EquippedWeapon->FireDelay
	);
}

void UCombatComponent::FireTimerFinished()
{
	if (EquippedWeapon == nullptr) return;
	bCanFire = true;
	if (bFiring && EquippedWeapon->bAutomatic)
	{
		Fire();
	}
	if (EquippedWeapon->IsEmpty())
	{
		Reload();
	}
}

bool UCombatComponent::CanFire()
{
	if (EquippedWeapon == nullptr) return false;
	return !EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_Unoccupied;
}

void UCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, StartingRocketAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, StartingARAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Shotgun, StartingShotgunAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, StartingPistolAmmo);
	
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

		if (TargetCharacter)
		{
			float DistanceToCharacter = (TargetCharacter->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
		}
		
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);
		if (!TraceHitResult.bBlockingHit) TraceHitResult.ImpactPoint = End;
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

int32 UCombatComponent::AmountToReload()
{
	if (EquippedWeapon == nullptr) return 0;
	int32 RoomInMag = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo();
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		int32 Least = FMath::Min(RoomInMag, AmountCarried);
		return FMath::Clamp(RoomInMag, 0, Least);
	}
	return 0;
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr) return;
	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	if (TargetCharacter && TargetCharacter->GetFollowCamera())
	{
		TargetCharacter->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::Reload()
{
	if (CarriedAmmo > 0 && CombatState != ECombatState::ECS_Reloading)
	{
		if (TargetCharacter == nullptr) return;
		CombatState = ECombatState::ECS_Reloading;
		UpdateAmmoValues();
		TargetCharacter->PlayReloadMontage();
	}
}

void UCombatComponent::FinishReloading()
{
	CombatState = ECombatState::ECS_Unoccupied;
	if (bFiring)
	{
		Fire();
	}
}
