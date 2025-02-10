// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
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
	friend class AShooterCharacter;
	void EquipWeapon(class AWeapon* WeaponToEquip);
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
	/** 
* HUD and crosshairs
*/
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
};
