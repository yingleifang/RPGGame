// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "ShooterAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UShooterAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	void AddCharacterAbilities(TSubclassOf<UGameplayAbility> AbilityClass);
	FGameplayEffectSpecHandle CreateDamageSpec(TSubclassOf<UGameplayEffect> DamageEffectClass,int32 AbilityLevel);

};
