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

