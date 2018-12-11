#include <iostream>
using namespace std;

int main(){
	int i = 42;
	const int &r = 42;
	const int &r2 = r+i;
	cout << r2 << endl;
	i = 52;
	cout << r2 << endl;
}
