// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Aura/TurningInPlace.h"
#include "ShooterCharacter.generated.h"


class UCameraComponent;

UCLASS()
class AURA_API AShooterCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const {return AttributeSet;}
	virtual void PossessedBy(AController* NewController) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	class AWeapon* OverlappingWeapon;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetOverlappingWeapon(AWeapon* Weapon);
	void PlayFireMontage(bool);
	void PlayHitReactMontage();

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
	FVector GetHitTarget() const;
	FRotator StartingAimRotation;
	AWeapon* GetEquippedWeapon();
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const {return TurningInPlace;}
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
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

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;
	
	UPROPERTY(EditAnywhere)
	UAnimMontage* HitReactMontage;
	
	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	ETurningInPlace TurningInPlace;

private:
	void TurnInPlace(float DeltaTime);
	virtual void Landed(const FHitResult& Hit) override;
	void ResetJump();
	void HideCameraIfCharacterClose();
};

