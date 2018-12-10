#include <iostream>
#include <bitset>

using namespace std;

int main(){
    string str("0011001");
    bitset<32> bs(str); //这里需切记所得bitset内存顺序是 1001100 000..000
    bitset<15> bs3(0xfff0); // 低位起赋值，最高位的1被砍
    for(int i = 0; i != bs3.size(); ++i){
        cout << bs3[i];
    }
    cout << endl;
    bitset<15> bs4("1111111111110000"); // 按字符串顺序从左到右反向转换（也即bitset从高位开始赋值），最后是最右边的0(bitset最低位)被抛弃
    for(int i = 0; i != bs4.size(); ++i){
        cout << bs4[i];
    }
    cout << endl;
    bitset<32> bs5(str, 5, 4); // 5th ~ 8th
    bitset<32> bs6(str, str.size() -4); // 倒数四个

    //两个等价
    bs6.flip(0);
    bs6[0].flip();
    //全部取反
    bs6.flip();
    return 0;
}