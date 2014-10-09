#include<iostream>
#include"CSA.h"
using namespace std;
int main(){
	CSA csa("bible");
	int num;
	csa.Counting("the",num);
	cout<<num<<endl;
	CSA c("bible");
	int counts=0;
	c=csa;
	c.Counting("the",counts);
	cout<<counts<<endl;
	
	return 0;
}
