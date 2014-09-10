#Adaptive-CSA

##What is it?
	Adaptive-CSA is a Succinct Data Structure(SDS),SDS can represent an
	object as implicitly,and in space close to information-theoretic 
	lower bound of the object while supporting operations of the 
	original object efficiently.
	CSA and FM both are implicit expression about SA(Suffix array),has 
	the ability of fast pattern mathing,and needs little space,and 
	Adapative-CSA is faster and smaller than Normal-CSA,and a advanced
	version of the original CSA.As you can guess,we developed a set of 
	coding methods,for each pieces of data,always select the best one,
	that's the bigest secret about my works.It works like a mixture of
	KMP and Bzip2,you can build a CSA-index for a document,and then you
	have mainly there operations in your hands:
	counting:count how many a pattern occurs in the document
	locating:locate all the positions where the pattern occurs
	extracting:decompress a piece of the document
##How to use it
###just for fun
	step 1:download it or clone it
	step 2:make
	step 3:run my_vsa
###build your own program 
	step 1:download it
	step 2:make
	step 3:include CSA.h
	step 4:g++ your_program.cpp -o xx -fm.a
###example
	```cpp
	#include"CSA.h"
	#include<iostream>
	using namespace std;
	int main()
	{

	```
##ChangeLog
