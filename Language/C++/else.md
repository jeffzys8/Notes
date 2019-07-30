> 存放一些C++的疑难杂症，以后归纳，可用于水文章 :)

# 对象声明歧义

- http://mk.oa.com/q/257389
- https://en.wikipedia.org/wiki/Most_vexing_parse


```c++
class A {};

int main()
{
    A a(A());
}
```

- 看似应该是这样的: 调用默认构造函数 --> 调用拷贝构造函数 --> 生成新对象；
- 但其实没有调用任何构造函数! 而是声明了一个函数 `function 'A a(A (*)())'`
- 也即：函数返回值为 `A`, 携带了一个参数，该参数是一个函数指针，指向返回值为A的空参函数
- 等价于 `A a(A func_name());`

> 据说可以用 C++11 的 列初始化 排除这种干扰；了解一下？

# 编译器优化类对象初始化过程

```cpp
class A
{
public:
    A()
    {
        cout << "default" << endl;
    }
    explicit A(const A &a)
    {
        cout << "copy" << endl;
    }
    A(const int &a) : a(a)
    {
        cout << "hello" << endl;
    }
    int a;
};

A func(){
    return A();
}

int main()
{
    A test = func();    
    // 逻辑上来说，会调用两次复制构造函数
    //      - func() 的 return 一次
    //      - test 初始化一次；
    // 因此设置为explicit编译器报两次错
    // 但去掉explicit, 运行的时候其实只调用了一次 default, 这是编译器优化了
}
```