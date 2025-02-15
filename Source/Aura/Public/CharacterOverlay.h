// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

class UTextBlock;
class UImage;

/**
 * 
 */
UCLASS()
class AURA_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoAmount;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CarriedAmmoAmount;
	
	UPROPERTY(meta = (BindWidget))
	UImage* ShugunImage;
	UPROPERTY(meta = (BindWidget))
	UImage* AssaultRifleImage;
	UPROPERTY(meta = (BindWidget))
	UImage* RocketLauncherImage;
};
