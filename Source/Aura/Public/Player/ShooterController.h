// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Controller.h"
#include "ShooterController.generated.h"
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class AURA_API AShooterController : public APlayerController
{
	GENERATED_BODY()
public:
	AShooterController();
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> ShooterContext;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveFowardAction;
	
	void MoveForward(const FInputActionValue& InputActionValue);
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveRightAction;

	void MoveRight(const FInputActionValue& InputActionValue);
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> LookUpAction;
	
	void LookUp(const FInputActionValue& InputActionValue);
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> TurnAction;
	
	void Turn(const FInputActionValue& InputActionValue);
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> PickupAction;
	void Pickup(const FInputActionValue& InputActionValue);

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> AimAction;
	void Aim(const FInputActionValue& InputActionValue);
	void StopAim(const FInputActionValue& InputActionValue);

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> JumpAction;
	void Jump(const FInputActionValue& InputActionValue);
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> FireAction;
	void Fire(const FInputActionValue& InputActionValue);
	void StopFire(const FInputActionValue& InputActionValue);

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ReloadAction;
	void Reload(const FInputActionValue& InputActionValue);

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> Weapon1;
	void Weapon1Action(const FInputActionValue& InputActionValue);

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> Weapon2;
	void Weapon2Action(const FInputActionValue& InputActionValue);

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> Weapon3;
	void Weapon3Action(const FInputActionValue& InputActionValue);

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> Weapon4;
	void Weapon4Action(const FInputActionValue& InputActionValue);
	
	
	UPROPERTY()
	class AShooterCharacter* ShooterOwnerCharacter;
	UPROPERTY()
	class AShooterController* ShooterOwnerController;

	UPROPERTY()
	class AShooterHud* ShooterHud;

};
