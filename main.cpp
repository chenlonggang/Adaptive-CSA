#include<iostream>
#include"CSA.h"
using namespace std;
int main(){
	
	CSA csa("bible");
	int num;
	csa.counting("the",num);
	cout<<num<<endl;
	uchar * p=NULL;
	csa.extracting(0,20,p);
	cout<<p<<endl;
	
	i32 * pos=NULL;
	csa.locating("the",num,pos);
	for(i32 i=0;i<10 && i<num;i++)
		cout<<pos[i]<<endl;

	return 0;
}
