// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Aura/TurningInPlace.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class AURA_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	class AWeapon* OverlappingWeapon;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetOverlappingWeapon(AWeapon* Weapon);
	void PlayFireMontage(bool);
	virtual void PostInitializeComponents() override;
	void EquipWeapon();
	void Aim();
	void StopAim();
	void Fire();
	void StopFire();
	bool IsWeaponEquipped();
	bool IsAiming();
	bool IsFiring();
	void AimOffset(float DeltaTime);
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const {return TurningInPlace;}
	FRotator StartingAimRotation;
	AWeapon* GetEquippedWeapon();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	float JumpCooldownDuration = 0.2f; // Example cooldown duration in seconds

	bool bCanJump = true;
	FTimerHandle JumpCooldownTimerHandle;
private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;
	
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;
	
	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);
	virtual void Landed(const FHitResult& Hit) override;
	void ResetJump();  
};

