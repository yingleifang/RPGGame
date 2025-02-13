// Copyright Druid Mechanics


#include "AbilitySystem/ShooterAbilitySystemComponent.h"


void UShooterAbilitySystemComponent::AddCharacterAbilities(
	TSubclassOf<UGameplayAbility> AbilityClass)
{
	FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);

	GiveAbility(AbilitySpec);
}

FGameplayEffectSpecHandle UShooterAbilitySystemComponent::CreateDamageSpec(
	TSubclassOf<UGameplayEffect> DamageEffectClass, int32 AbilityLevel)
{
	FGameplayEffectContextHandle EffectContext = MakeEffectContext();
	EffectContext.AddSourceObject(GetAvatarActor());
	return MakeOutgoingSpec(DamageEffectClass, AbilityLevel, EffectContext);
}
