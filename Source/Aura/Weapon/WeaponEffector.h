// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "WeaponTypes.h"
#include "Actor/AuraEffectActor.h"
#include "WeaponEffector.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AWeaponEffector : public AAuraEffectActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
protected:
	virtual void OnOverlap(AActor* TargetActor) override;
};
