#include<stdlib.h>
#include<sys/time.h>
#include<string.h>
#include"CSA.h"
#include<ctime>
#include<fstream>
#include<iostream>
using namespace std;
#define times 10000
int main(int argc, char ** argv){
	ifstream sources("path.txt");
	char  sourcefile[100];
	int filenum=atoi(argv[1]);
	int currentfile=0;
	struct timeval start;
	struct timeval end;
	for(currentfile=0;currentfile < filenum;currentfile++){
		sources.getline(sourcefile,200);
		cout<<"file:	"<<sourcefile<<endl;
		
		//unsigned char p[times][21]={'\0'};
		unsigned char * p[times];
		gettimeofday(&start,NULL);
		CSA csa(sourcefile);
		gettimeofday(&end,NULL);
		cout<<"	build-time: "<<(end.tv_sec-start.tv_sec)*1000000+end.tv_usec - start.tv_usec<<endl;

		int n= csa.getN();
		for(int i=0;i<times;i++){
			int x=rand()%(n-50);
			csa.extracting(x,20,p[i]);
		}

		//for counting
		int num=0;
		int sumnum=0;
		gettimeofday(&start,NULL);
		for(int i=0;i<times;i++){
			csa.counting((const char *)p[i],num);
			sumnum=sumnum+num;
//			cout<<i<<endl;
		}
//		cout<<sumnum<<endl;
		gettimeofday(&end,NULL);
		double count_time = (end.tv_sec-start.tv_sec)*1000000.0+end.tv_usec - start.tv_usec;
		cout<<"	count-time: "<<(count_time)/times<<endl;
		cout<<"	come-ratio: "<<csa.sizeInByteForCount()/(n*1.0)*8<<endl;
		for(int i=0;i<times;i++)
			delete [] p[i];
		//csa.counting("the",num);
		//cout<<num<<endl;
	}
	return 0;
}


