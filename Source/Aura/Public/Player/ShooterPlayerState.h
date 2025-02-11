// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "ShooterPlayerState.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class AURA_API AShooterPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AShooterPlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const {return AttributeSet;}

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;	
	
};
