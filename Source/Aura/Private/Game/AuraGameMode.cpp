// Copyright Druid Mechanics


#include "Game/AuraGameMode.h"

DEFINE_LOG_CATEGORY(LogAuraGameMode);

AAuraGameMode::AAuraGameMode()
{
}

void AAuraGameMode::BeginPlay()
{
	Super::BeginPlay();
#if WITH_GAMELIFT
	InitGameLift();
	#endif
}

void AAuraGameMode::SetServerParams(FServerParameters& OutserverParameters)
{
	//AuthToken returned from the "aws gamelift get-compute-auth-token" API. Note this will expire and require a new call to the API after 15 minutes.
	if (FParse::Value(FCommandLine::Get(), TEXT("-authtoken="), OutserverParameters.m_authToken))
	{
		UE_LOG(LogAuraGameMode, Log, TEXT("AUTH_TOKEN: %s"), *OutserverParameters.m_authToken)
	}

	//The Host/compute-name of the GameLift Anywhere instance.
	if (FParse::Value(FCommandLine::Get(), TEXT("-hostid="), OutserverParameters.m_hostId))
	{
		UE_LOG(LogAuraGameMode, Log, TEXT("HOST_ID: %s"), *OutserverParameters.m_hostId)
	}

	//The Anywhere Fleet ID.
	if (FParse::Value(FCommandLine::Get(), TEXT("-fleetid="), OutserverParameters.m_fleetId))
	{
		UE_LOG(LogAuraGameMode, Log, TEXT("FLEET_ID: %s"), *OutserverParameters.m_fleetId)
	}

	//The WebSocket URL (GameLiftServiceSdkEndpoint).
	if (FParse::Value(FCommandLine::Get(), TEXT("-websocketurl="), OutserverParameters.m_webSocketUrl))
	{
		UE_LOG(LogAuraGameMode, Log, TEXT("WEBSOCKET_URL: %s"), *OutserverParameters.m_webSocketUrl)
	}

	//The PID of the running process
	OutserverParameters.m_processId = FString::Printf(TEXT("%d"), GetCurrentProcessId());
	UE_LOG(LogAuraGameMode, Log, TEXT("PID: %s"), *OutserverParameters.m_processId)
}

void AAuraGameMode::ParseCommandLinePort(int32& OutPort)
{
	TArray<FString> CommandLineTokens;
	TArray<FString> CommandLineSwitches;
	FCommandLine::Parse(FCommandLine::Get(), CommandLineTokens, CommandLineSwitches);
	for (const FString& Switch : CommandLineSwitches)
	{
		FString Key;
		FString Value;
		if (Switch.Split("=", &Key, &Value))
		{
			if (Key.Equals(TEXT("port"), ESearchCase::IgnoreCase))
			{
				OutPort = FCString::Atoi(*Value);
				return;
			}
		}
	}
}

void AAuraGameMode::InitGameLift()
{
	UE_LOG(LogAuraGameMode, Log, TEXT("Initializing the GameLift Server"));

	FGameLiftServerSDKModule* GameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
	FServerParameters serverParameters;

	SetServerParams(serverParameters);

	GameLiftSdkModule->InitSDK(serverParameters);

	auto OnGameSession = [=](Aws::GameLift::Server::Model::GameSession gameSession)
	{
		FString GameSessionId = FString(gameSession.GetGameSessionId());
		UE_LOG(LogAuraGameMode, Log, TEXT("GameSession Initializing: %s"), *GameSessionId);
		GameLiftSdkModule->ActivateGameSession();
	};
	ProcessParameters.OnStartGameSession.BindLambda(OnGameSession);

	auto OnProcessTerminate = [=]()
	{
		UE_LOG(LogAuraGameMode, Log, TEXT("Game Server process is terminating."));
		GameLiftSdkModule->ProcessEnding();
	};
	ProcessParameters.OnTerminate.BindLambda(OnProcessTerminate);
	
	auto OnHealthCheck = []() 
	{
		UE_LOG(LogAuraGameMode, Log, TEXT("Performing Health Check"));
		return true;
	};
	ProcessParameters.OnHealthCheck.BindLambda(OnHealthCheck);
	
	int32 Port = FURL::UrlConfig.DefaultPort;
	ParseCommandLinePort(Port);
	ProcessParameters.port = Port;
	
	TArray<FString> LogFiles;
	LogFiles.Add(TEXT("Aura/Saved/Logs/Aura.log"));
	ProcessParameters.logParameters = LogFiles;
	UE_LOG(LogAuraGameMode, Log, TEXT("Calling Process Ready."));
	GameLiftSdkModule->ProcessReady(ProcessParameters);
}
