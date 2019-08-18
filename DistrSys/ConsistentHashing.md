# 一致性哈希算法

- [学习链接1](https://www.cnblogs.com/study-everyday/p/8629100.html)
- [更规范的Chord描述](https://blog.csdn.net/chen77716/article/details/6059575)

- 多个Redis结点进行 kv 缓存, 可以利用 `hash(x) % n` 来确定用哪个redis结点
- 某个结点删除了怎么办?! 原先的hash算法失效了, 一大堆的缓存血崩了!
- 新的结点加入也成了难题!

使用一致性哈希算法!

## 算法内容

- 对2^32取模
- 所有hash出来的值组成一个Hash环
- 把服务器结点hash出来放到环上
- 新到的数据也hash，然后沿着环走，找到的第一个结点就是所需的服务器结点
- 一致性Hash算法对于节点的增减都只需重定位环空间中的一小部分数据，具有较好的容错性和可扩展性

> 这样一个一个走很慢! 而且这么理解其实没有得到chord的精髓

## Chord

- 每个结点的finger表