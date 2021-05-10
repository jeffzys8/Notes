# 测试驱动开发

- [money example](money.md)

## TDD实践方式

> 测试驱动开发是个控制对编程的焦虑感的有效办法，在巨大困难的编程任务之间不断重复着建立目标并完成它的过程

**测试驱动开发的循环步骤(red/green/refactor)：**
1. 写一个不能通过的测试(一个描述接口的故事)
   - 先从调用的方式(接口)出发
   - 从最顶层的API开始设计测试并回推
2. 使用最简单的办法实现这个测试
   - 先解决代码可用，再解决整洁问题
3. 重构以去除冗余
   - 冗余是代码耦合的表现, 也是导致bug的一个原因.
   - 在测试通过以后, 消除冗余
   - 在冗余消除以后才可以从todo-list中消除某个待办事项

**Best practice**
- 在编程时使用to-do list
  - 基本上是想到什么要实现的或者需要改善的都往todos上面加一笔
- 发现缺少测试时，补全所发现缺失的测试 
  - 缺少测试的重构会在不经意间出问题
  - 重构应该是基于已有测试的代码, 否则就是重写了.
- 三角法：用两个不同的测试对同一段功能进行一般化定位
  - ```java
    public void testEquality() {
        assertTrue(new Dollar(5).equals(new Dollar(5)));
        assertFalse(new Dollar(5).equals(new Dollar(6)));
    }
    ```

## 重构方式

> 这应该到《重构》里面详细展开; 实际上应该先读重构这本书

尽量使用Value Object
- Value Object 不需要担心别名问题
- 对Value Object的所有操作需要返回一个新的object

解耦用户代码对子类的可见性，在父类创建工厂函数来解耦

提取公共类并将子类中的公共部分移至父类

> 为啥这么执着于去掉币种子类..是因为子类的很多逻辑相同所以要去掉是吗..确实是这样, 如果保留子类意味着每加入一种货币就要引入一堆重复代码

在有测试规定程序预期行为的前提下, 如果不确定实现或者重构(消除冗余)具体该怎么做, 可以不断缩小编码步骤让测试逐渐可运行, 在思路更加明确以后再做更大的改动. TDD给予开发者选择基于更小步骤开发的控制权

> 实现加法这里感觉作者有点跳, 先搬出来 `Expression interface`, 然后才去实现 `Sum`, `Money` 的 `reduce()`; 如果真按照TDD, 我觉得第一步应该还是先实现出 `Money` 的 `reduce`, 然后去通过新的测试引导出 `Sum` 这个类以及其 `reduce`, 然后再抽象出一个`Expression`接口来去除`Money`和`Sum`之间的冗余。最后才是出现`Bank`这个对象来参与`reduce`操作，接受`Expression`接口来统一处理

圈复杂度

设计模式
插入式对象, 插入式选择器
**递归组合模式**, 会和现实的对象概念有所偏离, 但对于简化代码很有帮助

内联方法来看看多层抽象以后到底调用了什么, 以免往错误的方向去重构

## (打个岔) golang实现类java抽象类

> 参考自 [neoyeelf.github.io](http://neoyeelf.github.io/2019/04/07/golang%E4%B9%9F%E8%83%BD%E5%AE%9E%E7%8E%B0%E6%8A%BD%E8%B1%A1%E7%B1%BB%E4%BA%86%EF%BC%9F/)
其实最重要的区别在于，golang的接口是没办法像Java的抽象类那样，也具备成员变量和成员函数的

```java
public abstract class Game
{
    public void play()
    {
        System.out.println(this.name() + " is awesome!");
    }
    public abstract String name();
}
public class Dota extends Game
{
    public String name()
    {
        return "Dota";
    }
}
public class LOL extends Game
{
    public String name()
    {
        return "LOL";
    }
}
```
对于golang, 并没有抽象函数这样的概念，可以换个角度，通过传参的方式把“子类”传进来

```Go
type Game struct {} // 父类
func (g *Game) play(game IGame) {
    fmt.Printf("%s is awesome!", game.Name())
}

type IGame interface {  // “桥”
    Name() string
}

type LOL struct { // 子类1 - 继承Game并实现IGame
    Game
} 
func (*LOL)Name() string{
    return "LOL"
}

type Dota struct { // 子类2 - 继承Game并实现IGame
    Game
}
func (*Dota)Name() string{
    return "Dota"
}
```

于是就可以使用出抽象类的感觉了：
```Go
dota := Dota()
dota.play(dota)

lol := LOL()
lol.play(lol)
```

当然，在TDD中要进一步解耦客户代码和子类的接触，要在父类新建一个静态工厂方法。
对于golang这是实现不了的..但可以通过在包内新建一个工厂方法来实现。

补充：interface和abstract class的区别在于，abstract class是知道了这个类的部分行为和属性，认为继承它的子类都是一致的 —— 比如抽象类“哺乳动物”都是胎生；而接口只是说明能干什么，类比到这个例子就是“知道这个对象能生育”，但并没有定义怎么生。所以如果要纯接口实现上述抽象，就得引入一个“哺乳动物”类。听起来好像也就多了一步？但关键在于，抽象类不能被实例化，而通过接口实现去引入父类“哺乳动物”就可以实例化了。