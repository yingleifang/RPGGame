// Copyright Druid Mechanics


#include "Player/ShooterPlayerState.h"
#include "AbilitySystem/ShooterAttributeSet.h"
#include "AbilitySystem/ShooterAbilitySystemComponent.h"


AShooterPlayerState::AShooterPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UShooterAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* AShooterPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
