#include <iostream>
using namespace std;


int main(){
    int i = 42, j = 1024;
    int *pi = &i, *pj = &j;
    *pj = *pi * * pj;   //最后的*是解引用，不是python
    cout << *pi << endl << *pj << endl;
    *pi *= *pi;
    cout << *pi << endl;

    int cA = 3;
    const int *const  ptCa = &cA;
    
    // *ptCa = 5; //报错
    cout << cA << endl;

    typedef int *pt_type;   //int是pt_type的解引用，因此pt_type是int的指针
    pt_type pt_m = &i;
    cout << *pt_m << endl;

    typedef int *pint;    //pint是int的指针
    const pint c = &i; 
    // 这里不能直接文本翻译为 const int *c
    // 事实上，const修饰了pint，所以c应该是一个const指针
    // 写作 int *const c;

    return 0;
}