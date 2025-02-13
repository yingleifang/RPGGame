// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Components/ActorComponent.h"
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
	
	friend class AShooterCharacter;
	TSubclassOf<UGameplayAbility> EquipWeapon(class AWeapon* WeaponToEquip);
	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> EquippedWeaponClass;
private:
	class AShooterCharacter* TargetCharacter;
	class AShooterController* TargetController;
	class AShooterHud* TargetHud;
	
	AWeapon* EquippedWeapon = nullptr;
	
	UPROPERTY(EditAnywhere, Category="Combat")
	class UAnimMontage* FireWeaponMongtage;

private:
	void SetAiming(bool);
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
	
};
