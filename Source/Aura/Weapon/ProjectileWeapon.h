// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire(const FVector& HitTarget) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;
	
};
