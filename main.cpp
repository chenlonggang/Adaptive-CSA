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

//	cout<<6%3<<endl;
	i32 n= csa.getN();
	i32 size=csa.sizeInByte();
	cout<<(size*1.0)/n<<endl;
	cout<<(csa.sizeInByteForCount()*1.0)/n<<endl;
	return 0;
}
