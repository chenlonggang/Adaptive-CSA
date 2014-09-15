#Adaptive-CSA

##What is it?
	Adaptive-CSA is a Succinct Data Structure(SDS),SDS can represent an
	object as implicitly,and in space close to information-theoretic 
	lower bound of the object while supporting operations of the 
	original object efficiently.
	CSA and FM both are implicit expression about SA(Suffix array),has 
	the ability of fast pattern mathing,and needs little space,and 
	Adapative-CSA is faster and smaller than Normal-CSA,Adapative-CSA
	is a advanced version of the original CSA.As you can guess,we 
	developed a set of coding methods,for each pieces of data,always 
	select the best one,that's the bigest secret about my works.
	It works like a mixture of KMP and Bzip2,you can build a CSA-index 
	for a document,and then you have mainly there operations in your hands:
	
	counting:count how many a pattern occurs in the document
	locating:locate all the positions where the pattern occurs
	extracting:decompress a piece of the document
##How to use it
###just for fun
	step 1:download  or clone it
	step 2:make
	step 3:run my_csa
###build your own program 
	step 1:download or clone it
	step 2:make
	step 3:include CSA.h
	step 4:g++ your_program.cpp -o xx -csa.a
###example
	```cpp
	#include"CSA.h"
	#include<iostream>
	using namespace std;
	int main()
	{
		CSA csa("filename");
		int num;
		csa.Counting(pattern,num);
		cout<<"pattern "<<pattern<<" occs "<<num<<" times"<<endl;

		int *pos;
		csa.Locating(pattern,num,pos);
		cout<<"pattern "<<pattern<<" occs "<<num<<" times"<<endl;
		cout<<"all positions are:"<<endl;
		for(int i=0;i<num;i++)
			cout<<pos[i]<<endl;
		delete [] pos;
		pos=NULL;

		char * sequence;
		int start=0;
		int len=20;
		csa.Extracting(start,len,sequence);
		cout<<"T[start...start+len-1] is "<<sequence<<endl;
		//it's your duty to delete sequence;
		delete [] sequence;
		sequence =NULL;
		
		csa.Save("index.csa");
		CSA csa2;
		csa2.Load("index.csa");

		return 0;
	}

	```
##ChangeLog
	2014.9.14: Want to finsh the hybrid work in Czip, hybrid will help to make  
	           CSA small and faster. And decoding from left or right self-adapting  
			   may help a lot to speed,but we need to split the middle runs,
			   this may do little harm to space, but good for speed,  
			   that's what we need, and for Pure-Gamma-coding ,space will  
			   not increase,that's good.This time,when decoding gamma-sequence,  
			   don't want use the lookup tables, it do not help lot as we need,  
			   and it does great harm to the beauty of the code.
