#include <iostream>
#include <vector>
#include <string.h>

using namespace std;

class MyString
{
private:
    char *_data;
    size_t _len;
    void _init_data(const char *s)
    {
        _data = new char[_len + 1];
        memcpy(_data, s, _len);
        _data[_len] = '\0';
    }

public:
    MyString()
    {
        _data = NULL;
        _len = 0;
    }

    MyString(const char *p)
    {
        _len = strlen(p);
        _init_data(p);
    }

    // 拷贝构造函数
    MyString(const MyString &str)
    {
        _len = str._len;
        _init_data(str._data);  // 耗时的深拷贝操作
        std::cout << "Copy Constructor is called! source: " << str._data << std::endl;
    }

/* 
1. 形参的符号必须是右值引用符号，即“&&”。

2. 形参不可以是常量，因为我们需要修改右值。

3. 传递过来的参数的资源链接和标记必须修改。否则，右值的析构函数就会释放资源。转移到新对象的资源也就无效了。
*/

    MyString(MyString &&str)    // 1. 2.
    {
        std::cout << "Move Constructor is called! source: " << str._data << std::endl;
        _len = str._len;
        _data = str._data;
        str._len = 0;       // 3.
        str._data = NULL;   // 3.
    }

    // 拷贝赋值操作符
    MyString &operator=(const MyString &str)
    {
        if (this != &str)
        {
            _len = str._len;
            _init_data(str._data);  // 耗时的深拷贝操作
        }
        std::cout << "Copy Assignment is called! source: " << str._data << std::endl;
        return *this;
    }

/*
同理
*/
    MyString &operator=(MyString &&str)
    {
        std::cout << "Move Assignment is called! source: " << str._data << std::endl;
        if (this != &str)
        {
            _len = str._len;
            _data = str._data;
            str._len = 0;
            str._data = NULL;
        }
        return *this;
    }

    virtual ~MyString()
    {
        if (_data)
            delete _data;
    }
};

int main()
{
    MyString a;
    a = MyString("Hello");  // 旧: 调用拷贝赋值; 新: 调用转移赋值
    std::vector<MyString> vec;
    vec.push_back(MyString("World"));  // 旧: 调用拷贝构造; 新: 调用转移构造
}
