// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Aura/TurningInPlace.h"
#include "Interaction/CombatInterface.h"
#include "Aura/Weapon/CombatState.h"
#include "ShooterCharacter.generated.h"


class UCameraComponent;
class UGameplayAbility;

UCLASS()
class AURA_API AShooterCharacter : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShooterCharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const {return AttributeSet;}
	ECombatState GetCombatState() const;
	virtual void PossessedBy(AController* NewController) override;
	FOnDeathSignature OnDeathDelegate;
	FOnASCRegistered OnAscRegistered;
	FOnDamageSignature OnDamageDelegate;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FTaggedMontage> AttackMontages;
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	class AWeapon* OverlappingWeapon;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UNiagaraSystem* BloodEffect;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetOverlappingWeapon(AWeapon* Weapon);
	void PlayFireMontage();
	void PlayHitReactMontage();
	void PlayReloadMontage();

	virtual void PostInitializeComponents() override;
	void EquipWeapon();
	void Aim();
	void StopAim();
	void Fire();
	void StopFire();
	bool IsWeaponEquipped();
	bool IsAiming();
	bool IsFiring();
	void Reload();
	void AimOffset(float DeltaTime);
	FVector GetHitTarget() const;
	FRotator StartingAimRotation;
	AWeapon* GetEquippedWeapon();
	
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const {return TurningInPlace;}
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	/** Combat Interface */
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;	
	virtual void Die(const FVector& DeathImpulse) override;
	virtual FOnDeathSignature& GetOnDeathDelegate() override;
	// virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;
	// virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	// virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	// virtual int32 GetMinionCount_Implementation() override;
	// virtual void IncremenetMinionCount_Implementation(int32 Amount) override;
	// virtual ECharacterClass GetCharacterClass_Implementation() override;
	virtual FOnASCRegistered& GetOnASCRegisteredDelegate() override;
	// virtual USkeletalMeshComponent* GetWeapon_Implementation() override;
	// virtual void SetIsBeingShocked_Implementation(bool bInShock) override;
	// virtual bool IsBeingShocked_Implementation() const override;
	virtual FOnDamageSignature& GetOnDamageSignature() override;
	/** end Combat Interface */

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
	float JumpCooldownDuration = 0.2f; // Example cooldown duration in seconds

	bool bCanJump = true;
	FTimerHandle JumpCooldownTimerHandle;

private:
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;
	
	UPROPERTY(EditAnywhere)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* ReloadMontage;
	
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

