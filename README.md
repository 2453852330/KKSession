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

