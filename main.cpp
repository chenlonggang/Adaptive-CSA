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
	csa.save("bible.csa");

	CSA csa2;
	csa2.load("bible.csa");
	uchar * pp=NULL;
	csa2.extracting(0,20,pp);
	cout<<pp<<endl;

	CSA csa3=csa2;
	int count=0;
	csa3.counting("the",count);
	cout<<count<<endl;
	uchar *ppp=NULL;
	csa3.extracting(0,20,ppp);
	cout<<ppp<<endl;
	
	csa3=csa;
	int cishu;
	csa3.counting("the",cishu);
	cout<<cishu<<endl;
	uchar * pppp=NULL;
	csa3.extracting(0,20,pppp);
	cout<<pppp<<endl;
	i32 * pos=NULL;
	csa3.locating("the",num,pos);
	for(i32 i=0;i<10 && i<num;i++)
		cout<<pos[i]<<endl;
	return 0;
}
