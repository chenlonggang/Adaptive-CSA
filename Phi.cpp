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
Phi::Phi(i32 * phiarray,i32 n,i32 bs){
	this->n=n;
	this->b=bs;
	this->a=18*b;
	
	value=new i32[n];
	memcpy(value,phiarray,n*sizeof(i32));
	/*
	value=phiarray;
	*/
	methodsAndSpace();
	allocAndInit();

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
				if(runs==b){
					methods->SetValue(j/b,2);//ALL1
					len=len+0;//该快的大小位0.
				}
				else if(runs>0){//该快不全是1,但是最后是一个1runs.
					rlg=rlg+2*blogsize(2*runs)-1;
				}
				if(rlg<g){ //rlg
					methods->SetValue(j/b,1);
					len=len+rlg;
				}
				else{//gamma
					methods->SetValue(j/b,0);
					len=len+g;
				}
				pre=value[j];
				runs=rlg=g=0;
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

void Phi::allocAndInit(){
	this->superoffset=new i32[lenofsuperoffset];
	this->offset=new InArray(n/b+1,blogsize(maxsbs));
	this->samples=new InArray(n/b+1,blogsize(n));
	this->sequence=new u32[lenofsequence];
	memset(superoffset,0,sizeof(i32)*lenofsuperoffset);
	memset(sequence,0,sizeof(u32)*lenofsequence);
}






