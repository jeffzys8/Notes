#include <iostream>

using namespace std;

template <typename T> void test(T&& i)
{
    cout << "   inside func:" << &i << endl;
}

int main(){
    int a = 3;
    cout << "auto var:" << &a << endl;
    test(a);
    const int v = 3;
    cout << "const var:" << &v << endl;
    test(v);
    test(3);
}