// Fill out your copyright notice in the Description page of Project Settings.


#include "KKSessionSubsystem.h"
#include "OnlineSubsystemUtils.h"

#define SESSION_NAME_KEY TEXT("SESSION_NAME_KEY")

void UKKSessionSubsystem::KK_CreateSession(UObject*obj,FName SessionName,FKKOnlineSessionSettings SessionSettings,FKKOnCreateSessionFinish OnCreateSessionFinish)
{
	IOnlineSubsystem * Subsystem = Online::GetSubsystem(obj->GetWorld());
	check(Subsystem!=nullptr)
	IOnlineSessionPtr OnlineSessionPtr = Subsystem->GetSessionInterface();
	
	// OnlineSessionPtr->AddOnCreateSessionCompleteDelegate_Handle();
	OnlineSessionPtr->OnCreateSessionCompleteDelegates.AddLambda([&OnlineSessionPtr,OnCreateSessionFinish](FName SessionName,bool bSuccess)
	{
		UE_LOG(LogTemp,Warning,TEXT("Create %s : %s"),*SessionName.ToString(),bSuccess?TEXT("Success"):TEXT("Failed"));
		OnlineSessionPtr->DumpSessionState();
		UE_LOG(LogTemp,Warning,TEXT("/****************************************************************************/"));
		OnCreateSessionFinish.ExecuteIfBound();
	});
	
	SessionSettings.OnlineSessionSettings.Set(SESSION_NAME_KEY,SessionName.ToString(),EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionPtr->CreateSession(0,SessionName,SessionSettings.OnlineSessionSettings);
}

void UKKSessionSubsystem::KK_FindSession(UObject* obj)
{
	IOnlineSubsystem * Subsystem = Online::GetSubsystem(obj->GetWorld());
	check(Subsystem!=nullptr)
	IOnlineSessionPtr OnlineSessionPtr = Subsystem->GetSessionInterface();

	OnlineSessionPtr->OnFindSessionsCompleteDelegates.AddLambda([this,obj](bool bSuccess)
	{
		int32 num = SearchedSession->SearchResults.Num();
		UE_LOG(LogTemp,Warning,TEXT("Search Sesseion : %s | find %d Session info"),bSuccess?TEXT("Success"):TEXT("Failed"),num);
		KK_JoinSession(obj);
	});
	SearchedSession = MakeShareable(new FOnlineSessionSearch);
	SearchedSession->bIsLanQuery = true;
	OnlineSessionPtr->FindSessions(0,SearchedSession.ToSharedRef());
}

void UKKSessionSubsystem::KK_JoinSession(UObject* obj)
{
	IOnlineSubsystem * Subsystem = Online::GetSubsystem(obj->GetWorld());
	check(Subsystem!=nullptr)
	IOnlineSessionPtr OnlineSessionPtr = Subsystem->GetSessionInterface();

	if (SearchedSession->SearchResults.Num() <= 0)
	{
		return;
	}

	OnlineSessionPtr->OnJoinSessionCompleteDelegates.AddLambda([this,OnlineSessionPtr,obj](FName SessionName, EOnJoinSessionCompleteResult::Type Type)
	{
		UE_LOG(LogTemp,Warning,TEXT("Join Session [%s] [%s]"),*SessionName.ToString(),*KK_GetJoinSessionResult(Type));
		if (Type == EOnJoinSessionCompleteResult::Success)
		{
			FString url;
			if (OnlineSessionPtr->GetResolvedConnectString(SessionName,url))
			{
				obj->GetWorld()->GetFirstPlayerController()->ClientTravel(url,ETravelType::TRAVEL_Absolute);
			}
			
		}
	});
	
	FString SessionName;
	SearchedSession->SearchResults[0].Session.SessionSettings.Get(SESSION_NAME_KEY,SessionName);
	OnlineSessionPtr->JoinSession(0,FName(*SessionName),SearchedSession->SearchResults[0]);
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
