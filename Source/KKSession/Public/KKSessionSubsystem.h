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

DECLARE_DYNAMIC_DELEGATE(FKKOnCreateSessionFinish);

UCLASS()
class KKSESSION_API UKKSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/*************************************** create *************************************/
	UFUNCTION(BlueprintCallable,Category="KKSession",meta=(WorldContext=obj))
	void KK_CreateSession(UObject*obj,FName SessionName,FKKOnlineSessionSettings SessionSettings,FKKOnCreateSessionFinish OnCreateSessionFinish);

	
	

	

	UFUNCTION(BlueprintCallable,Category="KKSession",meta=(WorldContext=obj))
	void KK_FindSession(UObject*obj);

	TSharedPtr<FOnlineSessionSearch> SearchedSession;

	UFUNCTION(BlueprintCallable,Category="KKSession",meta=(WorldContext=obj))
	void KK_JoinSession(UObject*obj);


	/*************************************** helper *************************************/
	FString KK_GetJoinSessionResult(EOnJoinSessionCompleteResult::Type Type);
};
