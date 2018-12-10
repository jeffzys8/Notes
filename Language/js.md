# 参考资料

- JavaScript 高级程序设计 (3rd edition)
    - [源码](http://www.wrox.com/WileyCDA/WroxTitle/Professional-JavaScript-for-Web-Developers-3rd-Edition.productCd-1118026691,descCd-DOWNLOAD.html)
- [JavaScript 教程 - 阮一峰](https://wangdoc.com/javascript/)

# JavaScript概述

Javascript 包括三个部分
- ECMAScript: 语法核心
- DOM: 文档对象模型
- BOM: 浏览器对象模型


# 基本语法

## 变量

- 不使用var会成为全局
- 变量提升

## 区块

- 对于`var`命令来说，JS的区块不构成单独的作用域（JS单独使用区块不常见，往往搭配for, if, while...）

## 条件语句

- 可以习惯将常量放左边，避免`==`写成`=`的错误
- switch需要break; case中可以写表达式； 
- switch中表达式与case比较采用的是`===`，即不能发生类型转换
- for 3个参数都可省略

## 循环结构

- `break` + [label] 可以用于跳出多重循环, 区块

    ```JS
    top:
        for (var i = 0; i < 3; i++) {
            for (var j = 0; j < 3; j++) {
                if (i === 1 && j === 1) break top; //跳出双重循环
                console.log('i=' + i + ', j=' + j);
            }
        }
    ```

    ```JS
    foo: {
        console.log(1);
        break foo;
        console.log('本行不会输出');
    }
    console.log(2);
    ```

- `continue` + [label] 可以用于跳出**本轮**外层循环
    ```JS
    top:
        for (var i = 0; i < 3; i++) {
            for (var j = 0; j < 3; j++) {
                if (i === 1 && j === 1) continue top;
                console.log('i=' + i + ', j=' + j);
            }
        }
    // i=0, j=0
    // i=0, j=1
    // i=0, j=2
    // i=1, j=0
    // i=2, j=0
    // i=2, j=1
    // i=2, j=2
    ```

# 数据类型

## 六种数据类型


- **原始类型**: number, string, boolean
- **合成类型**: object
    - 单个对象(object) (由原始类型组成)
    - array
    - function
- **两个特殊值**: undefined, null （含义很类似）

## 确定值的类型

- `typeof`
- `instanceof`
- `Object.prototype.toString`

单个对象、数组会返回`object`, 而函数会返回`function`, 但`function`的确是`object`的实例

## number

- 数值全以 64位浮点数 存储；注意精度误差; 
   ```JavaScript
    0.1 + 0.2 === 0.3
    // false

    0.3 / 0.1
    // 2.9999999999999996

    (0.3 - 0.2) === (0.2 - 0.1)
    // false
    ```

    ```js
    Math.pow(2, 53)
    // 9007199254740992
    
    Math.pow(2, 53) + 1
    // 9007199254740992
    ```
- 数太大的结果是`infinity`, 它是一个`number`对象
- 数太接近于0返回0
- `NaN`: not a number, 不等于任何值； `isNan()` 判断； `value !== value` 更为可靠
- 分母为0 返回的不是 `NaN`, 而是 `Infinity`
- `parseInt(string, 进制)`, `parseFloat`

## string

## object

- 不用变量名指向同一个对象，则指向同一内存地址; 原始类型值则是对值的拷贝
    ```js
    var o1 = {};
    var o2 = o1;

    o1.a = 1;
    o2.a // 1
    ```
- 点运算符-字符串；方括号运算符-变量
    ```js
    var foo = 'bar';
    var obj = {
    foo: 1,
    bar: 2
    };

    obj.foo  // 1
    obj[foo]  // 2
    ```
- 查看属性值，返回一个数组 `Object.keys()`
- 删除属性值，`Object.delete()`
- 属性是否存在 `in`, 会包括继承属性; `Object.hasOwnProperty()` 不包括继承属性
- `for...in`遍历属性（包括继承属性），但例如`toString`这样的**不可遍历属性**不会出现
- `with`(不建议使用): 块级域内暴露属性，方便书写；不可在内部创建新的属性


## function 

两种声明方法：
- function命令
    ```js
    function print(s){console.log(s);}
    ```
- (重要) 函数表达式
    ```js
    var print = funcction print(){console.log(s);}
    ```
    右边的print可以省略，但保留有利于debug & 内部自调用<br>
    给出一个自调用递归的例子, 斐波那契数列:
    ```js
    function fib(num) {
        if (num === 0) return 0;
        if (num === 1) return 1;
        return fib(num - 2) + fib(num - 1);
    }
    ```
- `function.name()`返回函数名; <br>`function.length()`返回参数个数;<br>`function.toString()`返回函数源码

    变相实现多行字符串:
    ```js
    var multiline = function (fn) {
    var arr = fn.toString().split('\n');
    return arr.slice(1, arr.length - 1).join('\n');
    };

    function f() {/*
    这是一个
    多行注释
    */}

    multiline(f);
    // " 这是一个
    //   多行注释"
    ```

- 作用域: 只存在全局and函数 (ES6新增块级，暂不讨论)<br>
  一个有趣的例子:
    ```js
    var a = 1;
    var x = function () {
        console.log(a);
    };

    function f() {
        var a = 2;
        x();
    }

    f() // 1
    ```

- 函数内部读取所有参数: `arguments`对象；`arguments.length()`查看参数个数
- 立即调用 `(function(){...}());` 或者 `(function(){...})();` 外层圆括号把function从定义变成了表达式; 总之只要把语句变成了表达式就可以执行
- 危险的操作 `eval()`
- **函数闭包**

    由于函数作用域的存在，内部函数可以访问外部函数的变量；而外部函数无法访问内部函数的变量；**把内部函数作为返回值**，就可以在全局访问外部函数的变量

    **闭包**: 能够读取其他函数内部变量的函数 (也即定义在一个函数内部的函数);

    用途:
    - 读取函数内部的变量
    - 使变量始终保存在内存中
    - (面对对象) 封装对象的私有属性和私有方法

    ```js
    function createIncrementor(start) {
        return function () {
            return start++;
        };
    }

    var inc = createIncrementor(5);

    inc() // 5
    inc() // 6
    inc() // 7
    ```




## array


# 面对对象

## 构造函数

```js
var Vehicle = function(){
    this.price = 1000;
}
```