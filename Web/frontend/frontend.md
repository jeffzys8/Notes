# 前端

这里用来记录一些前端的工程概念

## 生产环境构建

包括前端路由和后端路由怎么配合的一些思考，不要简单的`create-react-app`了以后就什么都不懂了。

https://www.zhihu.com/question/46630687


简单SPA发布：
```
php -S 192.168.3.14:3000 -t ./build/
```

## 微前端

暂时还看不懂，但搜这个的过程先简单描述一下：

我准备弄一个自己的博客系统，因此学了一下前端的生产环境构建。在这个过程中我想到一个问题，就是我现在写的就只是一个简单的SPA，`build`以后丢给Nginx并路由到对应资源就好。那对于大型应用来说，是不是采取的思路是将各个业务线(路由)作为一个个SPA分派给不同的前端团队开发，然后最后统一地修改Nginx路由来分配资源？问了一下泽章现在不是这样做的，让我去了解一下微前端，我就搜到了[美团的这篇文章](https://tech.meituan.com/2020/02/27/meituan-waimai-micro-frontends-practice.html)。不过现在还看不懂，以后随着经验的增长再来复盘吧。

