// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

	virtual void PostInitializeComponents() override;
	void EquipWeapon();
	void Aim();
	void StopAim();
	bool IsWeaponEquipped();
	bool IsAiming();
	
private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;
	
	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;
};
