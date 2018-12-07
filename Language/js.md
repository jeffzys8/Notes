# 参考资料

- JavaScript 高级程序设计 (3rd edition)
    - [源码](http://www.wrox.com/WileyCDA/WroxTitle/Professional-JavaScript-for-Web-Developers-3rd-Edition.productCd-1118026691,descCd-DOWNLOAD.html)
- [JavaScript 教程 - 阮一峰](https://wangdoc.com/javascript/)

# JavaScript概述

Javascript 包括三个部分
- ECMAScript: 语法核心
- DOM: 文档对象模型
- BOM: 浏览器对象模型


# 基础

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