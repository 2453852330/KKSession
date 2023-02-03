// Fill out your copyright notice in the Description page of Project Settings.


#include "KKSessionSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameSession.h"

#define SESSION_NAME_KEY TEXT("SESSION_NAME_KEY")

/*************************************** create *************************************/
void UKKSessionSubsystem::KK_CreateSession(APlayerController * PlayerController,FName SessionName,TMap<FName,FString> CustomData,FKKOnlineSessionSettings SessionSettings,FKKOnSessionExec OnCreateSessionFinish)
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
		OnCreateSessionFinish.ExecuteIfBound(bSuccess);
	});

	// add custom data
	SessionSettings.OnlineSessionSettings.Set(SESSION_NAME_KEY,SessionName.ToString(),EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	for (TTuple<FName,FString> it : CustomData)
	{
		SessionSettings.OnlineSessionSettings.Set(it.Key,it.Value,EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	}
	
	OnlineSessionPtr->CreateSession(0,SessionName,SessionSettings.OnlineSessionSettings);
}



/*************************************** find *************************************/
void UKKSessionSubsystem::KK_FindSession(APlayerController * PlayerController,FKKOnFindSessionExec OnFindSessionFinish)
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
		for (FOnlineSessionSearchResult it : SearchedSession->SearchResults)
		{
			tmp.Add(FKKOnlineSessionSearchResult(it));
		}
		OnFindSessionFinish.ExecuteIfBound(bSuccess,tmp);
	});
	
	SearchedSession = MakeShareable(new FOnlineSessionSearch);
	SearchedSession->bIsLanQuery = true;
	OnlineSessionPtr->FindSessions(0,SearchedSession.ToSharedRef());
}




/*************************************** join *************************************/
void UKKSessionSubsystem::KK_JoinSession(APlayerController * PlayerController,FKKOnSessionExec OnJoinSessionFinish)
{
	if (!PlayerController)
	{
		OnJoinSessionFinish.ExecuteIfBound(false);
		return;
	}
	
	IOnlineSubsystem * Subsystem = Online::GetSubsystem(PlayerController->GetWorld());
	check(Subsystem!=nullptr)
	IOnlineSessionPtr OnlineSessionPtr = Subsystem->GetSessionInterface();

	if (SearchedSession->SearchResults.Num() <= 0)
	{
		OnJoinSessionFinish.ExecuteIfBound(false);
		return;
	}

	OnlineSessionPtr->OnJoinSessionCompleteDelegates.Clear();
	OnlineSessionPtr->OnJoinSessionCompleteDelegates.AddLambda([this,OnJoinSessionFinish](FName SessionName, EOnJoinSessionCompleteResult::Type Type)
	{
		// save the session name to delete
		CacheSessionName = SessionName;
		OnJoinSessionFinish.ExecuteIfBound(Type==EOnJoinSessionCompleteResult::Success);
	});
	
	FString SessionName;
	SearchedSession->SearchResults[0].Session.SessionSettings.Get(SESSION_NAME_KEY,SessionName);
	OnlineSessionPtr->JoinSession(0,FName(*SessionName),SearchedSession->SearchResults[0]);
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
