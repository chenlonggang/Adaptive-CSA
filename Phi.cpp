/*============================================
# Filename: Phi.cpp
# Ver 1.0 2014-10-07
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
=============================================*/
#include"Phi.h"
#include<iostream>
using namespace std;
Phi::Phi(i32 * phiarray,i32 n,i32 bs){
	this->n=n;
	this->b=bs;
	this->a=18*b;
	this->index=0;
	value=phiarray;
	methodsAndSpace();
	allocAndInit();
	initTables();
	initCoders();
	codeAndFill();
//	MethodsStatic();	
	value=NULL;
}

bool Phi::checkCodeAndFill_getPhiArray(){
	i32 * array=getPhiArray();
	for(i32 i=0;i<n;i++)
		if(value[i]!=array[i]){
			cout<<value[i]<<" "<<array[i]<<" "<<i<<endl;
			return false;
		}
	return true;
}

void Phi::MethodsStatic(){
	i32 x[4]={0,0,0};
	for(i32 i=0;i<n/b;i++)
		x[methods->GetValue(i)]++;
	cout<<"gamma: "<<x[0]<<endl;
	cout<<"rlg:   "<<x[1]<<endl;
	cout<<"ALL1:  "<<x[2]<<endl;
	cout<<"rld:   "<<x[3]<<endl;
}

Phi::~Phi(){
	if(superoffset)	delete [] superoffset;
	if(samples)     delete samples;
	if(offset)      delete offset;
	if(sequence)    delete [] sequence;
	if(zerostable)  delete [] zerostable;
	samples=offset=NULL;
	superoffset=NULL;
	sequence=NULL;
	zerostable=NULL;
	delete coder0;
	delete coder1;
	delete coder2;
	delete coder3;
}


i32 Phi::write(savekit & s){
	s.writei32(n);
	s.writei32(a);
	s.writei32(b);
	//s.writei32(alphabetsize);
	//superoffset
	s.writei32(lenofsuperoffset);
	s.writei32array(superoffset,lenofsuperoffset);
	//offset
	offset->write(s);
	//samples
	samples->write(s);
	//sequence
	s.writei32(lenofsequence);
	s.writeu32array(sequence,lenofsequence);
	//methods
	methods->write(s);
	return 0;
}

i32 Phi::load(loadkit & s){
	s.loadi32(n);
	s.loadi32(a);
	s.loadi32(b);
	//superoffset
	s.loadi32(lenofsuperoffset);
	superoffset=new i32[lenofsuperoffset];
	s.loadi32array(superoffset,lenofsuperoffset);
	//offset
	offset=new InArray();
	offset->load(s);
	//samples
	samples=new InArray();
	samples->load(s);
	//sequence
	s.loadi32(lenofsequence);
	sequence = new u32[lenofsequence];
	s.loadu32array(sequence,lenofsequence);
	//methods
	methods=new InArray();
	methods->load(s);

	this->zerostable=new u8[1<<16];
	this->initTables();
	this->initCoders();
	return 0;
}

void Phi::initCoders(){
	coder0=new GAM(superoffset,offset,sequence,samples,
			zerostable,n,a,b,index);
	coder1=new RLG(superoffset,offset,sequence,samples,
			zerostable,n,a,b,index);
	coder2=new ALL1(superoffset,offset,sequence,samples,
			zerostable,n,a,b,index);
	coder3=new RLD(superoffset,offset,sequence,samples,
			zerostable,n,a,b,index);
}

i32 Phi::deltasize(i32 v){
	i32  x=blogsize(v);
	i32 pre=2*blogsize(x)-2;
	return pre+x-1;
}


i32 Phi::blogsize(i32 x){
	i32 len=0;
	while(x>0){
		x=(x>>1);
		len++;
	}
	return len;
}

i32 Phi::sizeInByte(){
	i32 part1=methods->GetMemorySize()+offset->GetMemorySize()+samples->GetMemorySize();
	i32 part2=lenofsuperoffset*sizeof(i32)+lenofsequence*sizeof(u32);
	return part1+part2;
}

void Phi::methodsAndSpace(){
	i32 totlen=0;  //总长度 
	i32 len=0;     //某个超快的编码长度
	i32 maxlen=0;  //所有超快中编码最长的哪一个
	i32 runs=0;    //runs长度
	i32 g=0;       //某块按照gamma编码时的编码长度
	i32 rlg=0;     //某块按照rl+gamma编码时的编码长度 
	i32 pre=0;     //pre=value[j-1],即当前值前面的那一个phi值
	i32 gap=0;     //
	methods = new InArray(n/b+1,2); //每个slot 2bits，可以表示4种编码方法 
	i32 x=n/a;
	i32 i=0;
	i32 j=0;

	i32 rld=0;
	i32 m=0;
	for(i=0;i<x+1;i++){
		for(j=i*a;j<(i+1)*a && j<n;j++){
			
			if(j%b==0){
				pre=value[j];
				rlg=0;
				g=0;
				rld=0;
				runs=0;
				continue;
			}
			gap=(value[j]-pre+n)%n;
			g=g+2*blogsize(gap)-1;
			if(gap==1)
				runs++;
			else{
				if(runs!=0)
				{
					rlg=rlg+2*blogsize(2*runs)-2;
					rld=rld+deltasize(2*runs);
				}
				rlg=rlg+2*blogsize(2*gap-1)-2;
				rld=rld+deltasize(2*gap-1);
				runs=0;
			}
			pre=value[j];
		
			if(j+1==n){//the last block,using gamma-coding
				methods->SetValue(j/b,0);
				len=len+g;
			}
			else{
				if((j+1)%b==0){
					if(runs >0){
						rlg=rlg+2*blogsize(2*runs)-2;
						rld=rld+deltasize(2*runs);
					}
					if(runs==b-1){//ALL1
						methods->SetValue(j/b,2);
						len=len+0;
					}
					else{
						m=min(g,min(rlg,rld));
						if(m==g){//gamma
							methods->SetValue(j/b,0);
							len=len+g;
						}
						else if(m==rlg){//rlg
							methods->SetValue(j/b,1);
							len=len+rlg;
						}
						else{//rld
							methods->SetValue(j/b,3);
							len=len+rld;
						}
					}
				}
			}
		}
		if(len>maxlen)
			maxlen=len;
		totlen=totlen+len;
		len=0;
	}
	lenofsequence=totlen/32+3;
	lenofsuperoffset=n/a+1;
	maxsbs=maxlen;
}

void Phi::initTables(){
	i32 D=16;
	for(i32 i=0;i<D;i++){
		for(i32 j=(1<<i);j<(2<<i);j++)
			zerostable[j]=D-1-i;
	}
	zerostable[0]=D;
}
	
void Phi::allocAndInit(){
	this->superoffset=new i32[lenofsuperoffset];
	this->offset=new InArray(n/b+1,blogsize(maxsbs));
	this->samples=new InArray(n/b+1,blogsize(n));
	this->sequence=new u32[lenofsequence];
	this->zerostable = new u8[1<<16];
	memset(zerostable,0,sizeof(u8)*(1<<16));
	memset(superoffset,0,sizeof(i32)*lenofsuperoffset);
	memset(sequence,0,sizeof(u32)*lenofsequence);
}

void Phi::codeAndFill(){
	i32 i=0;
	i32 len1=0;
	i32 len2=0;
	i32 index1=0;
	i32 index2=0;
	i32 runs=0;
	i32 pre=0;
	i32 gap=0;
	i32 method=0;
	for(i=0;i<n;i++){
		if(i%a==0){
			len2=len1;
			superoffset[index2]=len2;
			index2++;
		}
		if(i%b==0){
			samples->SetValue(index1,value[i]);
			offset->SetValue(index1,len1-len2);
			index1++;
			pre=value[i];
			method=methods->GetValue(i/b);
			if(method >3 || method <0 )
				cout<<"fuck "<<method<<" "<<i/b<<endl;
			runs=0;
			continue;
		}
		gap=value[i]-pre;
		if(gap<0)
			gap=gap+n;
		pre=value[i];

		switch(method){
			case 0:len1=len1+2*blogsize(gap)-1;coder0->encode(gap);break;
			case 1:
				   if(gap==1){
					   runs++;
					   if((i+1)%b==0|| (i+1)==n){
						   len1=len1+2*blogsize(2*runs)-2;
						   coder1->encode(2*runs);
						   runs=0;
					   }
				   }
				   else{
					   if(runs!=0){
						   len1=len1+2*blogsize(2*runs)-2;
						   coder1->encode(2*runs);
					   }
					   len1=len1+2*blogsize(2*gap-1)-2;
					   coder1->encode(2*gap-1);
					   runs=0;
				   };break;
			case 2:len1=len1+0;break;
			case 3:
				   if(gap==1){
					   runs++;
				       if((i+1)%b==0){
						   len1=len1+deltasize(2*runs);
						   coder3->encode(2*runs);
						   runs=0;
					   }
				   }
				   else{
					   if(runs!=0){
						   len1=len1+deltasize(2*runs);
						   coder3->encode(2*runs);
					   }
					   len1=len1+deltasize(2*gap-1);
					   coder3->encode(2*gap-1);
					   runs=0;
				   };break;
			default:cerr<<"347: method error"<<endl;exit(0);
		}
	}
}


i32 * Phi::getPhiArray(){
	i32 * phiarray = new i32[n];
	memset(phiarray,0,sizeof(i32)*n);
	i32 i=0;
	i32 method=0;
	i32 base=0;
	i32 position=0;
	i32 value=0;
	for(i=0;i<n;i++){
		if(i%b==0){
			base=samples->GetValue(i/b);
			method=methods->GetValue(i/b);
			phiarray[i]=base;
			continue;
		}
		switch(method){
			case 0:coder0->decode(position,value);
				   base=(base+value)%n;
				   phiarray[i]=base;
				   break;
			case 1:coder1->decode(position,value);
				   if(value%2==0){
					   i32 num=value/2;
					   for(i32 j=0;j<num;j++){
						   base=(base+1)%n;
						   phiarray[i+j]=base;
					   }
					   i=i+num-1;
				   }
				   else{
					   value=(value+1)/2;
					   base=(base+value)%n;
					   phiarray[i]=base;;
				   };
				   break;
			case 2://i32 ones=b-1;
				   for(i32 j=0;j<b-1;j++){
					   base=(base+1)%n;
					   phiarray[i+j]=base;
				   };
				   i=i+b-2;
				   break;
			case 3:
				   coder3->decode(position,value);
				   if(value%2==0){
					   i32 num=value/2;
					   for(i32 j=0;j<num;j++){
						   base=(base+1)%n;
						   phiarray[i+j]=base;
					   }
					   i=i+num-1;
				   }
				   else{
					   value=(value+1)/2;
					   base=(base+value)%n;
					   phiarray[i]=base;
				   };
				   break;
			default:cerr<<"method error"<<endl;exit(0);
		}
	}
	return phiarray;
}

bool Phi::checkgetValue(){
	for(int i=0;i<n;i++){
		if(value[i]!=getValue(i)){
			cout<<value[i]<<" "<<getValue(i)<<" "<<i<<endl;
			return false;
		}
	}
	return true;
}

i32 Phi::getValue(const i32 index){
	i32 base=samples->GetValue(index/b);
	i32 overloop=index%b;
	i32 position=superoffset[index/a]+offset->GetValue(index/b);
	i32 method=methods->GetValue(index/b);
	switch(method){
		case 0:return coder0->decodeAcc(position,base,overloop);
		case 1:return coder1->decodeAcc(position,base,overloop);
		case 2:return coder2->decodeAcc(position,base,overloop);
		case 3:return coder3->decodeAcc(position,base,overloop);
		default:cerr<<"method error"<<endl;exit(0);
	}
}


/*在区间[L,R]内，找到第一个Phi值大于等于l的
 */
/*
 一旦找到合法的block,查找的范围位((b-1)*L,b*L]  
 */
i32 Phi::leftBoundary(i32 pl,i32 l,i32 r){
	i32 L=b;
	i32 lb=(l+L-1)/L;
	i32 rb=r/L;
	i32 b=rb+1;
	i32 m=0;
	i32 x=0;
	while(lb<=rb){
		m=(lb+rb)>>1;
		x=samples->GetValue(m);
		if(x==pl){
			b=m;
			break;
		}
		if(x>pl){
			b=m;
			rb=m-1;
		}
		else
			lb=m+1;
	}
	if(b==0)
		return 0;
	i32 method = methods->GetValue(b-1);
	switch(method){
		case 0:return coder0->leftBoundary(b,l,r,pl);
		case 1:return coder1->leftBoundary(b,l,r,pl);
		case 2:return coder2->leftBoundary(b,l,r,pl);
		case 3:return coder3->leftBoundary(b,l,r,pl);
		default :cerr<<"method erroe"<<endl;exit(0);
	}
}


/*在区间[L,R]内，找到最后一个Phi值小于等于l的
 */
/*
   查找范围[b*L,(b+1)*L).
   */
i32 Phi::rightBoundary(i32 pr,i32 l,i32 r){
	i32 L=this->b;
	i32 lb=(l+L-1)/L;
	i32 rb=r/L;
	i32 b=lb-1;
	i32 m=0;
	i32 x=0;
	while(lb<=rb){
		m=(lb+rb)>>1;
		x=samples->GetValue(m);
		if(x==pr){
			b=m;
			break;
		}
		if(x<pr){
			b=m;
			lb=m+1;
		}
		else
			rb=m-1;
	}
	if(b<0)
		return 0;
	i32 method=methods->GetValue(b);
	switch(method){
		case 0:return coder0->rightBoundary(b,l,r,pr);
		case 1:return coder1->rightBoundary(b,l,r,pr);
		case 2:return coder2->rightBoundary(b,l,r,pr);
		case 3:return coder3->rightBoundary(b,l,r,pr);
		default:cerr<<"method error"<<endl;exit(0);
	}
}
