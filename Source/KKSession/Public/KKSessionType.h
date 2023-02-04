#pragma once
#include "OnlineSessionSettings.h"
#include "KKSessionType.generated.h"

USTRUCT(BlueprintType)
struct FKKOnlineSessionSettings
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName SessionName;
	// 公布的公开可用连接数
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 PublicConnectionsNum;
	// 仅专用（邀请密码）的连接数
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 PrivateConnectionsNum;
	// 此匹配是否在在线服务上公开发布
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bShouldAdvertise;
	// 是否允许加入进行中
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bAllowJoinInProgress;
	// 此游戏仅限局域网，外部玩家无法看到
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bIsLANMatch;
	// 专用服务器
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bIsDedicated;
	// 比赛是否应收集统计数据
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bUsesStats;
	// 匹配是否允许此会话的邀请
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bAllowInvites;
	// 是否显示用户状态信息
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bUsesPresence;
	// 是否允许通过玩家加入
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bAllowJoinViaPresence;
	// 是否仅允许好友通过玩家状态加入
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bAllowJoinViaPresenceFriendsOnly;
	// 服务器是否采用反欺诈（punkbuster、vac等）
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bAntiCheatProtected;
	
	FKKOnlineSessionSettings()
	{
		PublicConnectionsNum = 4;
		PrivateConnectionsNum = 0;
		bShouldAdvertise = true;
		bAllowJoinInProgress = true;
		bIsLANMatch = true;
		bIsDedicated = false;
		bUsesStats = true;
		bAllowInvites = true;
		bUsesPresence = true;
		bAllowJoinViaPresence = true;
		bAllowJoinViaPresenceFriendsOnly = false;
		bAntiCheatProtected = false;
	}
};

USTRUCT(BlueprintType)
struct FKKOnlineSessionSearchResult
{
	GENERATED_USTRUCT_BODY()
	FOnlineSessionSearchResult SearchResult;

	FKKOnlineSessionSearchResult(){}
	FKKOnlineSessionSearchResult(FOnlineSessionSearchResult InOnlineSessionSearchResult):SearchResult(InOnlineSessionSearchResult){}
};


USTRUCT(BlueprintType)
struct FKKOnlineSearchParam
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bLAN;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 MaxSearchResults;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float SearchTimeOut;
	// 暂时不添加过滤
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FString> Filter;
	FKKOnlineSearchParam():bLAN(true),MaxSearchResults(0),SearchTimeOut(10.f){}
};
