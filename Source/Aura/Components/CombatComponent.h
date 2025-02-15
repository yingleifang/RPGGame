// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Components/ActorComponent.h"
#include "Aura/Weapon/WeaponTypes.h"
#include "Aura/Weapon/CombatState.h"
#include "CombatComponent.generated.h"

#define TRACE_LENGTH 80000.f

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AURA_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	void SetHudCrosshairs(float DeltaTime);
	int32 AmountToReload();

	/** Accumulated recoil offset applied to the camera */
	FRotator CurrentRecoil;
	
	// Camera shake class to trigger on fire
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	TSubclassOf<UCameraShakeBase> FireCameraShakeClass;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	/** 
* Aiming and FOV
*/
	// Field of view when not aiming; set to the camera's base FOV in BeginPlay
	float DefaultFOV;
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV = 30.f;
	float CurrentFOV;
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;
	void InterpFOV(float DeltaTime);
	void Reload();
	UFUNCTION(BlueprintCallable, Category = Combat)
	void FinishReloading();
	
	friend class AShooterCharacter;
	TSubclassOf<UGameplayAbility> EquipWeapon(class AWeapon* WeaponToEquip);
	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> EquippedWeaponClass;
	class AShooterHud* TargetHud;

	UPROPERTY()
	AWeapon* PistolWeapon = nullptr;
	UPROPERTY()
	AWeapon* ShotgunWeaopn= nullptr;
	UPROPERTY()
	AWeapon* AssaultRifleWeapon= nullptr;
	UPROPERTY()
	AWeapon* RocketLauncherWeapon= nullptr;
	
	TMap<EWeaponType, int32> CarriedAmmoMap;
	
	void UpdateAmmoValues();
	
private:
	class AShooterCharacter* TargetCharacter;
	class AShooterController* TargetController;
	
	AWeapon* EquippedWeapon = nullptr;
	
	UPROPERTY(EditAnywhere, Category="Combat")
	class UAnimMontage* FireWeaponMongtage;

private:
	void SetAiming(bool);
	void Fire();
	void SetFiring(bool);

	
private:
	bool bAiming;
	bool bFiring;
	FVector HitTarget;
	/** 
* HUD and crosshairs
*/
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;

	FTimerHandle FireTimer;
	bool bCanFire = true;
	void StartFireTimer();
	void FireTimerFinished();
	bool CanFire();
	UPROPERTY()
	int32 CarriedAmmo;

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 80;
	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo = 8;
	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo = 999;
	UPROPERTY(EditAnywhere)
	int32 StartingShotgunAmmo = 10;
	
	
	void InitializeCarriedAmmo();

	ECombatState CombatState = ECombatState::ECS_Unoccupied;
};

