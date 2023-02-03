// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KKSessionType.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KKSessionSubsystem.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_DELEGATE_OneParam(FKKOnSessionExec,bool,bSuccess);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FKKOnFindSessionExec,bool,bSuccess,const TArray<FKKOnlineSessionSearchResult> &,FindSessionList);

UCLASS()
class KKSESSION_API UKKSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/*************************************** create *************************************/
	UFUNCTION(BlueprintCallable,Category="KKSession",meta=(AutoCreateRefTerm="CustomData"))
	void KK_CreateSession(APlayerController * PlayerController ,FName SessionName,TMap<FName,FString> CustomData ,FKKOnlineSessionSettings SessionSettings,FKKOnSessionExec OnCreateSessionFinish);

	
	/*************************************** find *************************************/
	UFUNCTION(BlueprintCallable,Category="KKSession")
	void KK_FindSession(APlayerController * PlayerController,FKKOnFindSessionExec OnFindSessionFinish);

	TSharedPtr<FOnlineSessionSearch> SearchedSession;

	/*************************************** join *************************************/
	UFUNCTION(BlueprintCallable,Category="KKSession")
	void KK_JoinSession(APlayerController * PlayerController,FKKOnSessionExec OnJoinSessionFinish);

	/*************************************** destory *************************************/
	UFUNCTION(BlueprintCallable,Category="KKSession")
	void KK_DestorySession(APlayerController * PlayerController,FKKOnSessionExec OnDestorySessionFinish);


	/*************************************** travel *************************************/
	
private:
	/*************************************** helper *************************************/
	FString KK_GetJoinSessionResult(EOnJoinSessionCompleteResult::Type Type);

	/*************************************** cache info *************************************/
	FName CacheSessionName;
};
