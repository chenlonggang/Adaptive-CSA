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
	
	value=new i32[n];
	memcpy(value,phiarray,n*sizeof(i32));
	/*
	value=phiarray;
	*/
	methodsAndSpace();
	allocAndInit();
	initZeroTable();
	codeAndFill();

	MethodsStatic();
}

void Phi::MethodsStatic(){
	i32 x[3]={0,0,0};
	for(i32 i=0;i<n/b;i++)
		x[methods->GetValue(i)]++;
	cout<<"gamma: "<<x[0]<<endl;
	cout<<"rlg:   "<<x[1]<<endl;
	cout<<"ALL0:  "<<x[2]<<endl;
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

}

i32 Phi::getValue(const i32 index){
	return value[index];
}

i32 Phi::load(loadkit & h){
	return 0;
}

i32 Phi::write(savekit & h){
	return 0;
}

i32 Phi::blogsize(i32 x){
	i32 len=0;
	while(x>0){
		x=(x>>1);
		len++;
	}
	return len;
}

/*
   计算编码方法和所需的空间.
*/
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
	for(i=0;i<x+1;i++){
		for(j=i*a;j<(i+1)*a && j<n;j++){
			
			if(j%b==0){
				pre=value[i];
				continue;
			}
			
			gap=value[j]-pre;
			if(gap<0)
				gap=gap+n;
			g=g+2*blogsize(gap)-1;
			if(gap==1)
				runs++;
			else{
				if(runs!=0)
					rlg=rlg+2*blogsize(2*runs)-1;
				rlg=rlg+2*blogsize(2*gap-3)-1;
				runs=0;
			}
			pre=value[j];
			
			if((j+1)%b==0 || (j+1)==n){
				if(runs==b){//ALL1
					methods->SetValue(j/b,2);
					len=len+0;
				}
				else if(runs>0){
					rlg=rlg+2*blogsize(2*runs)-1;
				}
				if(rlg<g){//rlg
					methods->SetValue(j/b,1);
					len=len+rlg;
				}
				else{//gamma
					methods->SetValue(j/b,0);
					len=len+g;
				}
			}
		}
		if(len>maxlen)
			maxlen=len;
		totlen=totlen+len;
		len=0;
	}
	lenofsequence=totlen/32+1;
	lenofsuperoffset=n/a+1;
	maxsbs=maxlen;
}

void Phi::initZeroTable(){
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
	this->zerostable = new u16[1<<16];
	memset(zerostable,0,sizeof(u16)*(1<<16));
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
			runs=0;
			continue;
		}
		gap=value[i]-pre;
		if(gap<0)
			gap=gap+n;
		pre=value[i];
		if(method==0){//gamma编码
			len1=len1+2*blogsize(gap)-1;
			Append(gap);
		}
		else if(method==1){//rl+g
			if(gap==1){
				runs++;
				if((i+1)%b==0){
					len1=len1+2*blogsize(2*runs)-1;
					Append(2*runs);
					runs=0;
				}
			}
			else{
				if(runs!=0){
					len1=len1+2*blogsize(2*runs)-1;
					Append(2*runs);
				}
				len1=len1+2*blogsize(2*gap-3)-1;
				Append(2*gap-3);
				runs=0;
			}
		}
		else if(method==2){ //all1
			len1=len1+0;
		}
		else
			cerr<<"error method"<<endl;
	}
}

void Phi::Append(i32 x){
	u64 y=x;
	i32 zeronums=blogsize(x)-1;
	index=index+zeronums;
	i32 valuewidth=zeronums+1;

	i32 anchor=(index>>5);
	i32 overloop =((anchor+2)<<5)-index-valuewidth;
	y=(y<<overloop);
	sequence[anchor]=(sequence[anchor]|(y>>32));
	sequence[anchor+1]=(sequence[anchor+1]|(y&0xffffffff));
	index=index+valuewidth;
}
	




