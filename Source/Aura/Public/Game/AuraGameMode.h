// Copyright Druid Mechanics

#pragma once

#include "CoreMinimal.h"
#include "AuraGameModeBase.h"
#include "GameLiftServerSDK.h"
#include "AuraGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAuraGameMode, Log, All);

/**
 * 
 */

UCLASS()
class AURA_API AAuraGameMode : public AAuraGameModeBase
{
	GENERATED_BODY()
public:
	AAuraGameMode();
protected:
	virtual void BeginPlay() override;

private:
	FProcessParameters ProcessParameters;
	void InitGameLift();
	void SetServerParams(FServerParameters& OutserverParameters);
	void ParseCommandLinePort(int32& OutPort);
	
};
