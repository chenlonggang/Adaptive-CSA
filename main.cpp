#include<iostream>
#include"CSA.h"
using namespace std;
int main(){
	CSA csa("bible");
	int num;
	csa.Counting("the",num);
	cout<<num<<endl;
	return 0;
}
