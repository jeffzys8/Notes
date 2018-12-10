#include <iostream>
using namespace std;

int A[4];

int main(){
    cout << A[0] << endl;   // 初始化为0
    int a[4];
    cout << a[1] << endl << a[3] << endl;   // 无初始化过程
    int b[4] = {};
    cout << b[1] << endl << b[3] << endl;   // 初始化为0
    return 0;
}