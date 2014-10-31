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
	step 4:g++ your_program.cpp -o xx csa.a
###example
	```cpp
	#include"CSA.h"
	#include<iostream>
	using namespace std;
	int main()
	{
		CSA csa("filename");
		int num;
		csa.counting(pattern,num);
		cout<<"pattern "<<pattern<<" occs "<<num<<" times"<<endl;

		int *pos;
		csa.locating(pattern,num,pos);
		cout<<"pattern "<<pattern<<" occs "<<num<<" times"<<endl;
		cout<<"all positions are:"<<endl;
		for(int i=0;i<num;i++)
			cout<<pos[i]<<endl;
		delete [] pos;
		pos=NULL;

		char * sequence;
		int start=0;
		int len=20;
		csa.extracting(start,len,sequence);
		cout<<"T[start...start+len-1] is "<<sequence<<endl;
		//it's your duty to delete sequence;
		delete [] sequence;
		sequence =NULL;
		
		csa.save("index.csa");
		CSA csa2;
		csa2.load("index.csa");

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
	2014.10.6: add class UseCount,Shallow Copy for operator = and copy cons for CSA  
	2014.10.7: the framwork is Ok,what to do next is to make the query functions work  
				right,and then,focus on Phi class  
	2014.10.8: Save and Load function for CSA is OK.Phi class is on the way.
	           I design three code/decode mehod:three are:pure-gamma,rl+gamma,ALL1,
			   both are good at different situations.  
	2014.10.8: code Phi array using three methods,it seems right...  
	2014.10.9: I am sure that code is right. leftboundary and rightboundary function
	           are on the way. For the backward search, first do a binary search on 
			   the sampled anchors,and then decode a block,the costs on binary search
			   and sequence-decode diffs not very much,so decoding from right or left
			   side will not help a lot,so,forget it!  
	2014.10.9: getValue for Phi is OK now...  
	2014.10.9: Boundary function is finshed,but is right?  
	2014.10.10:Boundary function seems right,because countSearch2 seems right.
	           next we need compare the results of counfSearch and countSearch2.
			   countSearch is a baseline,it's absolutely right!  
	2014.10.10:The size function is ok,it seems right.And add compressRatio function  
	2014.10.11:EveryThing seems right now. For highlyrepetive data,the block size 
	           is bigger,it is good to speed,and do little harm to query's speed.  
	2014.10.11:Adjust the threshold,so the block-size is more reasonable.This work 
	           is closed temporary,goodbye! 
	2014.10.26:It seems that lookuptables is helpful, so use it again. And fix some
	           bugs,about leftboundary ,rightboundary ,methodAndSpace and codeAndFill
			   it's humiliatory that everything seems not ok until now  
	2014.10.27:we can do something to make compression ratio better:
	           Now we use run-length like this:
			   if the value is a real gap, coding 2*value-3 using gamma-coding.
			   if the value is a 1's runs, coding 2*value using gamma-coding.
			   the ratio of gap not equals 1 is very low, especailly for highly-repetive
			   data, so the cost for 2*value-3 is trivial,but for 1's runs,the cost is
			   a littler high, especially for english-like and highly-repetive data,
			   there are many runs in these datas,so if we do like this,the cost will 
			   be smaller:
			   if the value is a real gap, the coding value is 2*value-1 now;
			   if the value is a runs,the coding value is 2*value, same as before.
			   because the low ratio,the change of real gap is trivial.
			   what important is: now all the coding value is >1,at least is 2,
			   for the gamma-coding,the 0's num  preceding the binary-bits of the 
			   value is at least 1, so we can change the gamma-coding's rule:
			   if the binary-bits's length is x,we append x-2 0s on the head,
			   now we will save 1 bit for every single value, compare with the 
			   original mapping method, the real gap will cost 1 bit more at most
			   time,but for the 1's runs, 1 bit will saved. 
			   so it will be helpful for highly-repetive data.
			   working on it now! In fact u8 for lookuptables is enough, fix it
	2014.10.28:delta coding will be better for run-length.
	2014.10.28:delta coding is really better than gamma coding for highly-repetive
	           data, and we use the new maping for real gap and runs.
	2014.10.28:shrink the lookup-tables may will be helpful to speed.
	2014.10.29:shrink tables help alittle,for xml file 10000 parten with length 20,
	           the average count time:45us-->40us,and fix a bug about shrink.
	2014.10.31:refactor, and the Adapative_CSA is closed

