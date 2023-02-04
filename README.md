# KKSession
ue4 session bp lib

## 1:

```
初次测试,只需要创建和搜索加入Session即可;而且可以自定义SessionName?
```

## 2:

````
完成销毁Session,必须自行缓存Session名称,因为无法获取到当前所在的Session名称;
同时因为Session代理使用Lambda,导致无法清除代理,每执行一次都会添加重复的执行函数;
````

## 3:

```
尝试寻找如何获取当前的SessionName,发现并没有获取方案;目前只能自行缓存;
使用 ServerTravel MapName 进行关卡切换,一切正常;
Lambda绑定待解决;
```

## 4:

```
发现使用 OnlineSessionPtr->OnJoinSessionCompleteDelegates.Clear(); 可以清除绑定的Lambda代理,所以不需要进行更新;
同时修复了主机Session断开,客户端Session依旧存在的问题;
```

## 5:

```
修改所有WorldContext为PlayerController,同时完善CreateSession,支持添加自定义参数;
待完善 FindSession
```

## 6:

```
修复了CreateSession参数异常bug;
完善了Session相关接口;
```

发现问题:

```
1.局域网LAN必须使用Name_GameSession作为CreateSession的name,不然读取到的人数就是错误的;
2.FOnlineSessionSearch的Session查询设置中过滤匹配对局域网无效;
```

## StartSession和EndSession的存在意义?

```
https://nerivec.github.io/old-ue4-wiki/pages/online-multiplayer.html
```

StartSession:

```
您可以并且通常应该向子系统发出信号，表明比赛已经开始，以防您不想列出正在进行的游戏或只列出正在进行的游戏。

ShooterGame中在游戏会话类中重写的 void HandleMatchHasStarted（） 方法;
因为当当前活动的游戏模式开始比赛时会调用此方法,它应该对所有连接的客户端上的方法进行 RPC 调用，以便它们也启动联机会话。在此示例中，服务器正在对名为 ClientStartOnlineGame 的方法执行 RPC
```

```cpp
// tell non-local players to start online game
for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
{
   AClientPlayerController* PC = Cast<AClientPlayerController>(*It);
   if (PC && !PC->IsLocalPlayerController())
   {
      PC->ClientStartOnlineGame();
   }
}
```

EndSession:

```
结束会话是通知子系统匹配已结束的操作。在这方面，它与开始会话相反。比赛结束后，游戏模式调用 GameSession->HandleMatchHasEnded（）; 这意味着在你的游戏会话子类中，你应该覆盖 void HandleMatchHasEnded（）。
```

```cpp
IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
if (Session.IsValid()) 
{
   Session->EndSession(GameSessionName);
}
```

```
这只显示了子系统对 EndSession 的调用，实际上这是非常不完整的。您需要告诉连接的客户端您的游戏会话已结束，以便他们可以正常处理您的比赛即将结束，以便他们也可以结束其会话。再次，ShooterGame以优雅的方式做到这一点。当比赛结束时，服务器在连接的控制器上调用 ClientEndOnlineGame（），这是一个复制的 RPC，它又会在每个连接的客户端控制器上调用此方法实现。
```

```cpp
// tell the clients to end
for (FConstPlayerControllerIterator It 
   = GetWorld()->GetPlayerControllerIterator(); It; ++It)
{
   AClientPlayerController* PC = Cast<AClientPlayerController>(*It);
   if (PC && !PC->IsLocalPlayerController())
   {
      PC->ClientEndOnlineGame();
   }
}
```

然后，每个客户端调用 EndSession 来结束其会话！

```cpp
IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
if(Session.IsValid())
{
   Session->EndSession(PlayerState->SessionName);
}
```

