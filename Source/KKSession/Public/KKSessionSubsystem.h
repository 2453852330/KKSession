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
	void KK_CreateSession(APlayerController * PlayerController,TMap<FName,FString> CustomData ,FKKOnlineSessionSettings SessionSettings,FKKOnSessionExec OnCreateSessionFinish);

	
	/*************************************** find *************************************/
	UFUNCTION(BlueprintCallable,Category="KKSession")
	void KK_FindSession(APlayerController * PlayerController,FKKOnlineSearchParam OnlineSearchParam,FKKOnFindSessionExec OnFindSessionFinish);

	TSharedPtr<FOnlineSessionSearch> SearchedSession;

	/*************************************** join *************************************/
	UFUNCTION(BlueprintCallable,Category="KKSession")
	void KK_JoinSession(APlayerController * PlayerController,FKKOnlineSessionSearchResult OnlineSessionSearchResult,FKKOnSessionExec OnJoinSessionFinish);

	/*************************************** destory *************************************/
	UFUNCTION(BlueprintCallable,Category="KKSession")
	void KK_DestorySession(APlayerController * PlayerController,FKKOnSessionExec OnDestorySessionFinish);
	
	/*************************************** start session*************************************/
	UFUNCTION(BlueprintCallable,Category="KKSession")
	void KK_StartSession(APlayerController * PlayerController,FKKOnSessionExec OnStartSessionFinish);

	/*************************************** end session *************************************/
	UFUNCTION(BlueprintCallable,Category="KKSession")
	void KK_EndSession(APlayerController * PlayerController,FKKOnSessionExec OnEndSessionFinish);
	
	/*************************************** update session *************************************/
	UFUNCTION(BlueprintCallable,Category="KKSession")
	void KK_UpdateSession(APlayerController * PlayerController,FKKOnlineSessionSettings OnlineSessionSettings, FKKOnSessionExec OnUpdateSessionFinish);
	
	/*************************************** travel *************************************/
	UFUNCTION(BlueprintCallable,Category="KKSession")
	void KK_ClientTravel(APlayerController * PlayerController);
	
	/*************************************** get session info *************************************/
	UFUNCTION(BlueprintCallable,BlueprintPure,Category="KKSession|Get")
	static FString KK_GetSessionInfo(UPARAM(ref) const FKKOnlineSessionSearchResult & SessionSearchResult);

	UFUNCTION(BlueprintCallable,BlueprintPure,Category="KKSession|Get")
	static FString KK_GetSessionUserName(UPARAM(ref) const FKKOnlineSessionSearchResult & SessionSearchResult);

	UFUNCTION(BlueprintCallable,BlueprintPure,Category="KKSession|Get")
	static int32 KK_GetSessionPing(UPARAM(ref) const FKKOnlineSessionSearchResult & SessionSearchResult);

	UFUNCTION(BlueprintCallable,BlueprintPure,Category="KKSession|Get")
	static int32 KK_GetSessionMaxPlayerNum(UPARAM(ref) const FKKOnlineSessionSearchResult & SessionSearchResult);

	UFUNCTION(BlueprintCallable,BlueprintPure,Category="KKSession|Get")
	static int32 KK_GetSessionCurrentPlayerNum(UPARAM(ref) const FKKOnlineSessionSearchResult & SessionSearchResult);

	UFUNCTION(BlueprintCallable,BlueprintPure,Category="KKSession|Get")
	static FString KK_GetSessionCustomData(UPARAM(ref) const FKKOnlineSessionSearchResult & SessionSearchResult,FName KeyName);

	/*************************************** debug *************************************/
	UFUNCTION(BlueprintCallable,Category="KKSession|Debug")
	static void KK_DebugSessionInfo(APlayerController * PlayerController);
private:
	/*************************************** helper *************************************/
	FString KK_GetJoinSessionResult(EOnJoinSessionCompleteResult::Type Type);
	
	/*************************************** cache info *************************************/
	FName CacheSessionName;

	/*************************************** make session settings *************************************/
	FOnlineSessionSettings Kk_MakeOnlineSessionSettings(FKKOnlineSessionSettings KKOnlineSessionSettings);
};

