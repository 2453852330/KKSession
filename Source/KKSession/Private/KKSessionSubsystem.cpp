// Fill out your copyright notice in the Description page of Project Settings.


#include "KKSessionSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameSession.h"

#define SESSION_NAME_KEY TEXT("SESSION_NAME_KEY")

/*************************************** create *************************************/
void UKKSessionSubsystem::KK_CreateSession(APlayerController * PlayerController,TMap<FName,FString> CustomData,FKKOnlineSessionSettings SessionSettings,FKKOnSessionExec OnCreateSessionFinish)
{
	if (!PlayerController)
	{
		OnCreateSessionFinish.ExecuteIfBound(false);
		return;
	}
	IOnlineSubsystem * Subsystem = Online::GetSubsystem(PlayerController->GetWorld());
	check(Subsystem!=nullptr)
	IOnlineSessionPtr OnlineSessionPtr = Subsystem->GetSessionInterface();
	

	OnlineSessionPtr->OnCreateSessionCompleteDelegates.Clear();
	// OnlineSessionPtr->AddOnCreateSessionCompleteDelegate_Handle();
	OnlineSessionPtr->OnCreateSessionCompleteDelegates.AddLambda([OnlineSessionPtr,OnCreateSessionFinish,this](FName SessionName,bool bSuccess)
	{
		// cache session name 
		CacheSessionName = SessionName;
		// Debug Session
		OnlineSessionPtr->DumpSessionState();
		
		OnCreateSessionFinish.ExecuteIfBound(bSuccess);
	});
	
	FOnlineSessionSettings OnlineSessionSettings = Kk_MakeOnlineSessionSettings(SessionSettings);
	
	// add custom data
	OnlineSessionSettings.Set(SESSION_NAME_KEY,SessionSettings.SessionName.ToString(),EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	for (TTuple<FName,FString> it : CustomData)
	{
		OnlineSessionSettings.Set(it.Key,it.Value,EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	}
	
	// 貌似查询参数过滤对局域网无效
	// OnlineSessionSettings.Set(SEARCH_KEYWORDS,FString(TEXT("QWER")),EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	// 因为局域网必须使用 NAME_GameSession,不然无法获取正确的人数;
	OnlineSessionPtr->CreateSession(0,SessionSettings.bIsLANMatch?NAME_GameSession:SessionSettings.SessionName,OnlineSessionSettings);
	
}



/*************************************** find *************************************/
void UKKSessionSubsystem::KK_FindSession(APlayerController * PlayerController,FKKOnlineSearchParam OnlineSearchParam,FKKOnFindSessionExec OnFindSessionFinish)
{
	TArray<FKKOnlineSessionSearchResult> tmp;
	if (!PlayerController)
	{
		OnFindSessionFinish.ExecuteIfBound(false,tmp);
		return;
	}
	
	IOnlineSubsystem * Subsystem = Online::GetSubsystem(PlayerController->GetWorld());
	check(Subsystem!=nullptr)
	IOnlineSessionPtr OnlineSessionPtr = Subsystem->GetSessionInterface();

	OnlineSessionPtr->OnFindSessionsCompleteDelegates.Clear();
	OnlineSessionPtr->OnFindSessionsCompleteDelegates.AddLambda([OnFindSessionFinish,this](bool bSuccess)
	{
		TArray<FKKOnlineSessionSearchResult> tmp;
		// 暂时不添加过滤
		for (FOnlineSessionSearchResult it : SearchedSession->SearchResults)
		{
			tmp.Add(FKKOnlineSessionSearchResult(it));
		}
		
		OnFindSessionFinish.ExecuteIfBound(bSuccess,tmp);
	});
	
	SearchedSession = MakeShareable(new FOnlineSessionSearch);
	SearchedSession->bIsLanQuery = OnlineSearchParam.bLAN;
	SearchedSession->MaxSearchResults = OnlineSearchParam.MaxSearchResults;
	
	// 对局域网无效,所以直接对搜索结果过滤完事;
	// SearchedSession->QuerySettings.Set(SEARCH_KEYWORDS,OnlineSearchParam.Filter[0],EOnlineComparisonOp::Equals);
	OnlineSessionPtr->FindSessions(0,SearchedSession.ToSharedRef());
}


/*************************************** join *************************************/
void UKKSessionSubsystem::KK_JoinSession(APlayerController * PlayerController,FKKOnlineSessionSearchResult OnlineSessionSearchResult,FKKOnSessionExec OnJoinSessionFinish)
{
	if (!PlayerController)
	{
		OnJoinSessionFinish.ExecuteIfBound(false);
		return;
	}
	
	IOnlineSubsystem * Subsystem = Online::GetSubsystem(PlayerController->GetWorld());
	check(Subsystem!=nullptr)
	IOnlineSessionPtr OnlineSessionPtr = Subsystem->GetSessionInterface();
	
	OnlineSessionPtr->OnJoinSessionCompleteDelegates.Clear();
	OnlineSessionPtr->OnJoinSessionCompleteDelegates.AddLambda([this,OnJoinSessionFinish](FName SessionName, EOnJoinSessionCompleteResult::Type Type)
	{
		// save the session name to delete
		CacheSessionName = SessionName;
		OnJoinSessionFinish.ExecuteIfBound(Type==EOnJoinSessionCompleteResult::Success);
	});

	// fix the session name is not right:
	if (OnlineSessionSearchResult.SearchResult.Session.SessionSettings.bIsLANMatch)
	{
		OnlineSessionPtr->JoinSession(0,NAME_GameSession,OnlineSessionSearchResult.SearchResult);
	}
	else
	{
		FString SessionName;
		OnlineSessionSearchResult.SearchResult.Session.SessionSettings.Get(SESSION_NAME_KEY,SessionName);
		OnlineSessionPtr->JoinSession(0,FName(*SessionName),OnlineSessionSearchResult.SearchResult);
	}
}



/*************************************** destory *************************************/
void UKKSessionSubsystem::KK_DestorySession(APlayerController * PlayerController,FKKOnSessionExec OnDestorySessionFinish)
{
	if (!PlayerController)
	{
		OnDestorySessionFinish.ExecuteIfBound(false);
		return;
	}
	
	IOnlineSubsystem * Subsystem = Online::GetSubsystem(PlayerController->GetWorld());
	check(Subsystem!=nullptr)
	IOnlineSessionPtr OnlineSessionPtr = Subsystem->GetSessionInterface();
	
	OnlineSessionPtr->OnDestroySessionCompleteDelegates.Clear();
	OnlineSessionPtr->OnDestroySessionCompleteDelegates.AddLambda([OnDestorySessionFinish](FName SessionName,bool bSuccess)
	{
		UE_LOG(LogTemp,Warning,TEXT("Destory [%s] [%s]"),*SessionName.ToString(),bSuccess?TEXT("Success"):TEXT("Failed"));
		OnDestorySessionFinish.ExecuteIfBound(bSuccess);
	});
	
	OnlineSessionPtr->DestroySession(CacheSessionName);
}

/*************************************** start session *************************************/
void UKKSessionSubsystem::KK_StartSession(APlayerController* PlayerController,FKKOnSessionExec OnStartSessionFinish)
{
	if (!PlayerController)
	{
		OnStartSessionFinish.ExecuteIfBound(false);
		return;
	}
	IOnlineSubsystem * Subsystem = Online::GetSubsystem(PlayerController->GetWorld());
	check(Subsystem!=nullptr)
	IOnlineSessionPtr OnlineSessionPtr = Subsystem->GetSessionInterface();

	OnlineSessionPtr->OnStartSessionCompleteDelegates.Clear();
	OnlineSessionPtr->OnStartSessionCompleteDelegates.AddLambda([OnStartSessionFinish](FName SessionName,bool bSuccess)
	{
		OnStartSessionFinish.ExecuteIfBound(bSuccess);
	});
	OnlineSessionPtr->StartSession(CacheSessionName);
}
/*************************************** end session *************************************/
void UKKSessionSubsystem::KK_EndSession(APlayerController* PlayerController, FKKOnSessionExec OnEndSessionFinish)
{
	if (!PlayerController)
	{
		OnEndSessionFinish.ExecuteIfBound(false);
		return;
	}
	IOnlineSubsystem * Subsystem = Online::GetSubsystem(PlayerController->GetWorld());
	check(Subsystem!=nullptr)
	IOnlineSessionPtr OnlineSessionPtr = Subsystem->GetSessionInterface();

	OnlineSessionPtr->OnEndSessionCompleteDelegates.Clear();
	OnlineSessionPtr->OnEndSessionCompleteDelegates.AddLambda([OnEndSessionFinish](FName SessionName,bool bSuccess)
	{
		OnEndSessionFinish.ExecuteIfBound(bSuccess);
	});
	OnlineSessionPtr->EndSession(CacheSessionName);
}

/*************************************** update session *************************************/
void UKKSessionSubsystem::KK_UpdateSession(APlayerController* PlayerController,FKKOnlineSessionSettings OnlineSessionSettings, FKKOnSessionExec OnUpdateSessionFinish)
{
	if (!PlayerController)
	{
		OnUpdateSessionFinish.ExecuteIfBound(false);
		return;
	}
	IOnlineSubsystem * Subsystem = Online::GetSubsystem(PlayerController->GetWorld());
	check(Subsystem!=nullptr)
	IOnlineSessionPtr OnlineSessionPtr = Subsystem->GetSessionInterface();

	OnlineSessionPtr->OnUpdateSessionCompleteDelegates.Clear();
	OnlineSessionPtr->OnUpdateSessionCompleteDelegates.AddLambda([OnUpdateSessionFinish](FName SessionName,bool bSuccess)
	{
		OnUpdateSessionFinish.ExecuteIfBound(bSuccess);
	});
	FOnlineSessionSettings tmp = Kk_MakeOnlineSessionSettings(OnlineSessionSettings);
	OnlineSessionPtr->UpdateSession(CacheSessionName,tmp);
}

/*************************************** travel *************************************/
void UKKSessionSubsystem::KK_ClientTravel(APlayerController * PlayerController)
{
	if (!PlayerController)
	{
		return;
	}
	IOnlineSubsystem * Subsystem = Online::GetSubsystem(PlayerController->GetWorld());
	check(Subsystem!=nullptr)
	IOnlineSessionPtr OnlineSessionPtr = Subsystem->GetSessionInterface();
	FString url;
	OnlineSessionPtr->GetResolvedConnectString(CacheSessionName,url);
	PlayerController->ClientTravel(url,ETravelType::TRAVEL_Absolute);
}

/*************************************** get session info *************************************/
FString UKKSessionSubsystem::KK_GetSessionInfo(const FKKOnlineSessionSearchResult & SessionSearchResult)
{
	return SessionSearchResult.SearchResult.Session.SessionInfo->ToString();
}

FString UKKSessionSubsystem::KK_GetSessionUserName(const FKKOnlineSessionSearchResult & SessionSearchResult)
{
	return SessionSearchResult.SearchResult.Session.OwningUserName;
}

int32 UKKSessionSubsystem::KK_GetSessionPing(const FKKOnlineSessionSearchResult& SessionSearchResult)
{
	return SessionSearchResult.SearchResult.PingInMs;
}

int32 UKKSessionSubsystem::KK_GetSessionMaxPlayerNum(const FKKOnlineSessionSearchResult& SessionSearchResult)
{
	return SessionSearchResult.SearchResult.Session.SessionSettings.NumPublicConnections;
}

int32 UKKSessionSubsystem::KK_GetSessionCurrentPlayerNum(const FKKOnlineSessionSearchResult& SessionSearchResult)
{
	return SessionSearchResult.SearchResult.Session.SessionSettings.NumPublicConnections - SessionSearchResult.SearchResult.Session.NumOpenPublicConnections;
}

FString UKKSessionSubsystem::KK_GetSessionCustomData(const FKKOnlineSessionSearchResult& SessionSearchResult,FName KeyName)
{
	FString tmp;
	SessionSearchResult.SearchResult.Session.SessionSettings.Get(KeyName,tmp);
	return tmp;
}

/*************************************** helper *************************************/
FString UKKSessionSubsystem::KK_GetJoinSessionResult(EOnJoinSessionCompleteResult::Type Type)
{
	FString tmp;
	switch (Type)
	{
	case EOnJoinSessionCompleteResult::Success:
		tmp = TEXT("Success");
		break;
	case EOnJoinSessionCompleteResult::UnknownError:
		tmp = TEXT("UnknownError");
		break;
	case EOnJoinSessionCompleteResult::AlreadyInSession:
		tmp = TEXT("AlreadyInSession");
		break;
	case EOnJoinSessionCompleteResult::SessionIsFull:
		tmp = TEXT("SessionIsFull");
		break;
	case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
		tmp = TEXT("CouldNotRetrieveAddress");
		break;
	case EOnJoinSessionCompleteResult::SessionDoesNotExist:
		tmp = TEXT("SessionDoesNotExist");
		break;
	}
	return tmp;
}

/*************************************** make online session settings *************************************/
FOnlineSessionSettings UKKSessionSubsystem::Kk_MakeOnlineSessionSettings(
	FKKOnlineSessionSettings KKOnlineSessionSettings)
{
	FOnlineSessionSettings tmp;
	tmp.bIsLANMatch = KKOnlineSessionSettings.bIsLANMatch;
	tmp.bAllowInvites = KKOnlineSessionSettings.bAllowInvites;
	tmp.bIsDedicated = KKOnlineSessionSettings.bIsDedicated;
	tmp.bShouldAdvertise = KKOnlineSessionSettings.bShouldAdvertise;
	tmp.bUsesPresence = KKOnlineSessionSettings.bUsesPresence;
	tmp.bUsesStats = KKOnlineSessionSettings.bUsesStats;
	tmp.bAntiCheatProtected = KKOnlineSessionSettings.bAntiCheatProtected;
	tmp.bAllowJoinInProgress = KKOnlineSessionSettings.bAllowJoinInProgress;
	tmp.bAllowJoinViaPresence = KKOnlineSessionSettings.bAllowJoinViaPresence;
	tmp.bAllowJoinViaPresenceFriendsOnly = KKOnlineSessionSettings.bAllowJoinViaPresenceFriendsOnly;
	tmp.NumPublicConnections = KKOnlineSessionSettings.PublicConnectionsNum;
	tmp.NumPrivateConnections = KKOnlineSessionSettings.PrivateConnectionsNum;
	return tmp;
}
