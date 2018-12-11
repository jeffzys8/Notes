#include <iostream>
using namespace std;

enum Test{ta=1,tb,tc=1,td};

int main(){
	Test a = ta;
	Test c = tc;
	if(a == c){
	cout << "same" << endl;
	}
}
