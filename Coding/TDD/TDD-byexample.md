# TDD - By Example

> 比较随意的笔记

测试驱动开发的循环步骤(red/green/refactor)：
1. 写一个不能通过的测试(一个描述接口的故事)
2. 使用最简单的办法实现这个测试
3. 重构以去除冗余

测试驱动开发是个控制对编程的焦虑感的有效办法，在巨大困难的编程任务之间不断重复着建立目标并完成它的过程

先解决代码可用，再解决整洁问题

在编程时使用to-do list (基本上是想到什么要实现的或者需要改善的都往todos上面加一笔)

从最顶层的API开始设计测试并回推

在一个设计缺陷(side effect)出现的时候，编写一个测例来覆盖这个缺陷，并基于此重新设计代码(使用Value Object)

- Value Object 不需要担心别名问题
- 对Value Object的所有操作需要返回一个新的object
- Value Object 需要实现 `equals()` 
- *实现`hashCode()`?

三角法，用两个不同的测试对同一段功能进行一般化定位
```java
public void testEquality() {
    assertTrue(new Dollar(5).equals(new Dollar(5)));
    assertFalse(new Dollar(5).equals(new Dollar(6)));
}
```

如果不在意识到需要测试的时候补上测试，未被覆盖的部分在重构时就很容易遇上问题

提取公共类并将子类中的公共部分移至父类

> 函数签名：参数的类型和个数，用于函数重载（java）

引入工厂方法，消除子类和客户代码之间的耦合


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