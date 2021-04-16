# Money Example

> 模仿 TDD - By Example 的 Money Example; 实现一个多币种结算的包

## TODO-list

- [x] 测试: 当 ￥:$ = 6.5:1 时, $5 + ￥13 * 2 = $9 = ￥58.5
- [x] 测试: $2.0 + $3.0 = $5.0
- [x] Dollar 工厂函数
- [x] Dollar 相等
- [x] 测试: ￥2.0 + ￥3.0 = ￥5.0
- [x] 消除对 dollar.Value 的引用
- [ ] 消除 Yuan Dollar的冗余
- [x] 消除对 Dollar 和 Yuan 结构体的直接引用
- [x] 测试: ￥2.0 * 2 = ￥5; $2.0 * 2 = $5
- [ ] 管理汇率的地方: bank
- [x] 返回currency: bank 要使用
- [x] $5 + ￥5 的表示
- [x] bank.Settle
- [ ] Reduce for all
- [ ] 二叉树 --> 多叉树
- [ ] Sum 和 Add 合并
- [ ] reverse rate
- [ ] 同个交换获得不同rate的风险
- [ ] 精度问题


## Thoughts

- 开始体会到Go让人不舒服的地方 - 对于多态支持的乏力; 对于 Dollar 和 Yuan的相似方法 `Add` `Equal` 之类，除非用反射否则就是要全部实现一遍；但是反射又在编码和运行时都增加了复杂度
- 利用未完成的test+todo-list来记录现场，让自己随时可以停下工作，又随时可以恢复工作，进度处于可把控的安心状态。
- 我在一开始并没有很好的办法将`Yuan` 和 `Dollar`的冗余消除(这不是Go的问题，是我自己的问题), 导致在功能完全实现的时候，改动一点会将所有测试都置于无法运行的状态。
- 利用组合的方式模拟抽象类`Money`，但最终目标是只留下抽象类 - https://zhuanlan.zhihu.com/p/33759346
