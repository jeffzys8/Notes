# PHP - note1

> 官方手册

## 一些入门例子

### **helloworld**:
```php
<html>
    <head>
        <title>Example</title>
    </head>
    <body>

        <?php
        echo "Hi, I'm a PHP script!";
        ?>

    </body>
</html>
```

可以留意到输出使用 `echo`

PHP脚本三个领域:
1. 服务端脚本 (PHP解析器)
2. daemon? (Linux--cron, Windows--TaskScheduler)
3. 桌面应用程序(不了)

需要运行在web服务器中 (apache, nginx), 然后安装php; Windows下可以用 XAMPP集成 Apache+PHP; (这么看来web服务器得支持php才行); 

可能需要一些好用的IDE (EMacs?)

### **变量 phpinfo()**

```php
<body>
    <?php echo $_SERVER['HTTP_USER_AGENT']; ?>
    <?php phpinfo(); ?>
</body>
```

在 PHP 中，变量总是以一个美元符开头; `$_SERVER` 是一个特殊的 PHP 保留变量，它包含了 web 服务器提供的所有信息，被称为超全局变量; [查阅](https://www.php.net/manual/zh/reserved.variables.server.php); `phpinfo();`也可以看到

### **流程与函数**

```php
<?php
if (strpos($_SERVER['HTTP_USER_AGENT'], 'MSIE') !== FALSE) {
    echo '正在使用 Internet Explorer。<br />';
}
?>
```

### **嵌套PHP与HTML**

```php
<?php
if (strpos($_SERVER['HTTP_USER_AGENT'], 'MSIE') !== FALSE) {
?>
<h3>strpos() 肯定没有返回假 (FALSE)</h3>
<p>正在使用 Internet Explorer</p>
<?php
} else {
?>
<h3>strpos() 肯定返回假 (FALSE)</h3>
<center><b>没有使用 Internet Explorer</b></center>
<?php
}
?>
```

> 不太懂为什么要用 `!==`; 另外要注意IE 现在不包括 MSIE字符串了, 有用到再说吧

### **表单**

hello.php
```php
<form action="action.php" method="post">
 <p>姓名: <input type="text" name="name" /></p>
 <p>年龄: <input type="text" name="age" /></p>
 <p><input type="submit" /></p>
</form>
```

action.php
```php
你好，<?php echo htmlspecialchars($_POST['name']); ?>。
你 <?php echo (int)$_POST['age']; ?> 岁了。
```

`htmlspecialchars()` 可以防止注入(xml?名字我忘了, 反正就是防止插入标签, 尤其是js代码); `$_REQUEST` 包含所有GET, POST, COOKIE 和 FILE的数据

// TODO: XForms?

// TODO: url后面的query都是`GET`方法传递的吗

## 基本语法

`<?php ?>`, 纯php不要加结束标记

### **数据类型**

- 基本类型: boolean, integer, float(double), string
- 符合类型: array, object, callable(可调用?)
- 特殊类型: resource(资源), NULL

// TODO: 伪类型, 伪变量

### 基本数据类型

变量类型通常是自获得的; `var_dump()`, `gettype()`, `is_type`:
```php
<?php
$a_bool = TRUE;   // 布尔值 boolean
$a_str  = "foo";  // 字符串 string
$a_str2 = 'foo';  // 字符串 string
$an_int = 12;     // 整型 integer

echo gettype($a_bool); // 输出:  boolean
echo gettype($a_str);  // 输出:  string

// 如果是整型，就加上 4
if (is_int($an_int)) {
    $an_int += 4;
}

// 如果 $bool 是字符串，就打印出来
// (啥也没打印出来)
if (is_string($a_bool)) {
    echo "String: $a_bool";
}
?>
```

整数溢出返回float; float强制转换为int向下取证

比较浮点数:
```php
$a = 1.23456789;
$b = 1.23456780;
$epsilon = 0.00001;

if(abs($a-$b) < $epsilon) {
    echo "true";
}
```

string最大可以2GB(厉害了); 单引号string中没有变量和 `\\` `\'` 之外的转义; 还有一种奇特的string表示法 heredoc:

```php
class foo
{
    var $foo;
    var $bar;

    function foo()
    {
        $this->foo = 'Foo';
        $this->bar = array('Bar1', 'Bar2', 'Bar3');
    }
}

$foo = new foo();
$name = 'MyName';

echo <<<EOT
My name is "$name". I am printing some $foo->foo.
Now, I am printing some {$foo->bar[1]}.
This should print a capital 'A': \x41
EOT;
```

注意上面的例子使用了 **花括号** 来涵盖复杂的变量

// TODO: nowdoc 和 herodoc, 'EOT'

跑个题: array太灵活了吧!
```php
$juices = array("apple", "orange", "koolaid1" => "purple");
echo "He drank some $juices[0] juice.".PHP_EOL;
echo "He drank some $juices[1] juice.".PHP_EOL;
echo "He drank some $juices[koolaid1] juice.".PHP_EOL;
```

还有这个吓到我了
```php
class foo {
    var $bar = 'I am bar.';
}

$foo = new foo();
$bar = 'bar';
$baz = array('foo', 'bar', 'baz', 'quux');
echo "{$foo->$bar}\n";
echo "{$foo->{$baz[1]}}\n";
// 都会输出 I am bar.
```

// TODO: {$} 变量解析似乎有不少内容

$类常量和静态变量:
```php
<?php
// 显示所有错误
error_reporting(E_ALL);

class beers {
    const softdrink = 'rootbeer';
    public static $ale = 'ipa';
}

$rootbeer = 'A & W';
$ipa = 'Alexander Keith\'s';

// 有效，输出： I'd like an A & W
echo "I'd like an {${beers::softdrink}}\n";

// 也有效，输出： I'd like an Alexander Keith's
echo "I'd like an {${beers::$ale}}\n";
?>
```

字符串可以用 `.` 连接起来 (注意不是`+`); 当然有很多其他内容需要查阅文档;

// TODO [字符串类型详情](https://www.php.net/manual/zh/language.types.string.php#language.types.string.details)(跳过)


//TODO Array