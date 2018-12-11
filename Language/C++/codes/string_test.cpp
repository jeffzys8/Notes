#include <iostream>

using namespace std;

int main(){
    string b = " world";
    string a = "hello" + string("sdf") + "sdf";         // 在中间新建了一个string 对象
    string c = "hello" + *(new string("sdf")) + "kk";   // 使用动态分配获得同样效果
    //要注意C++的动态分配不同于Java，它返回了这个对象的指针，不能直接作对象使用
    cout << c << endl;
    c.at(0) = 's';  //等价于c[0] = 's';
    cout << c << endl;

    for(int i = 0; i < c.size(); ++i)   //这里应该用 string::size_type
    {
        c[i] = toupper(c[i]);
    }
    cout << c << endl;

    for(basic_string<char>::const_iterator iter = c.begin(); iter != c.end(); ++iter){
        cout << *iter << endl;
    }
    

    
    return 0;
}