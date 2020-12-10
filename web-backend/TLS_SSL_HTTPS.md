# TLS, SSL, HTTPS

> 呼 https://blog.csdn.net/chan70707/article/details/82932153

## HTTPS 通信时序图

```
client                  server
        1 请求https连接
            ---->

        2 返回证书(包含公钥)
            <----
   
3 产生随机对称秘钥
   
4 公钥加密对称秘钥
    
        5 发送加密后的对称秘钥
            ---->

        6 加密发送具体http请求
            ---->
        7 加密发送具体http响应
            <----
```


## 证书

- [ ] 证书合法性验证

> 暂时理解为CA和client之间也是非对称加密，

CA 公钥(server 公钥 + 其他info) --> 证书
server 保存自己的证书，传给 client
client 找 CA验证证书: CA私钥(证书) = server公钥，核查 √ 通过

client 信任 CA; CA信任server

- 证书信息：过期时间和序列号
- 所有者信息：姓名等
- 所有者公钥