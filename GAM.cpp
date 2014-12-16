/*============================================
# Filename: GAM.cpp
# Ver 1.0 2014-11-17
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description:  coding for a sequence of gap of Phi.
=============================================*/
#include"GAM.h"
GAM::GAM(integer *superoffset,InArray *offset,u32 *sequence,
		InArray *samples,u8 * zerostable,integer n,integer a,
		integer b,integer &index):index(index){
	this->superoffset=superoffset;
	this->offset=offset;
	this->sequence=sequence;
	this->samples=samples;
	this->zerostable=zerostable;
	this->n=n;
	this->a=a;
	this->b=b;
	initTables();
}

GAM::~GAM(){
	if(decodevaluenum)
		delete [] decodevaluenum;
	if(decodebitnum)
		delete [] decodebitnum;
	if(decoderesult)
		delete [] decoderesult;
}

void GAM::encode(integer x){
	u64 y=x;
	integer zeronums=blogsize(x)-1;
	index=index+zeronums;
	integer valuewidth=zeronums+1;
/*	
	integer anchor=(index>>5);
	integer overloop =((anchor+2)<<5)-index-valuewidth;
	y=(y<<overloop);
	sequence[anchor]=(sequence[anchor]|(y>>32));
	sequence[anchor+1]=(sequence[anchor+1]|(y&0xffffffff));
	index=index+valuewidth;
*/
	integer anchor_start =(index>>5);
	integer anchor_end=((index+valuewidth)>>5);
	if(anchor_end-anchor_start<2){
		i32 overloop=((anchor_start+2)<<5)-index-valuewidth;
		y=(y<<overloop);
		sequence[anchor_start]=(sequence[anchor_start]|(y>>32));
		sequence[anchor_start+1]=(sequence[anchor_start+1]|(y&0xffffffff));
		index=index+valuewidth;
	}
	else{
		i32 s1=(anchor_start+1)*32-index;
		i32 s2=valuewidth-32-s1;
		sequence[anchor_start]=(sequence[anchor_start]|(y>>(valuewidth-s1)));
		sequence[anchor_start+1]=(sequence[anchor_start+1]|((y>>s2)&(0xffffffff)));
		sequence[anchor_start+2]=(sequence[anchor_start+2]|
				(((((1ULL<<s2)-1)&y)<<(32-s2))&(0xffffffff)));
		index=index+valuewidth;
	}
}

integer GAM::decode(integer & position,integer &value){
	integer a=this->zeroRun(position);
	value=getBits(position,a+1);
	position=position+a+1;
	return 2*a+1;
}

integer GAM::decodeAcc(integer position,integer base,integer num){
	integer i=0;
	integer value=0;
	while(i<num){
		decode(position,value);
		base=(base+value)%n;
		i++;
	}
	return base;
}

integer GAM::leftBoundary(integer b,integer l,integer r,integer pl){
	integer ans=0;
	integer m=0;
	integer SL=a;
	integer L=this->b;
	integer x=samples->GetValue(b-1);
	if(r>b*L-1)
		r=b*L-1;
	ans=r+1;
	m=(b-1)*L;
	integer position=superoffset[m/SL]+offset->GetValue(b-1);
	integer d=0;
	while(m<l){
		decode(position,d);
		x=(x+d)%n;
		m++;
	}

	integer p=0;
	integer num=0;
	integer bits=0;
	integer v=0;
	bool loop=false;
	while(x<pl && m<r){
		loop=true;
		p=getBits(position,16);
		num=this->decodevaluenum[p];
		if(num!=0){
			m=m+num;
			position=position+this->decodebitnum[p];
			v=this->decoderesult[p];
			v=(v==0?(256):v);
			x=(x+v)%n;
		}
		else{
			m++;
			bits=this->decode(position,d);
			x=x+d;
		}
	}
	if(loop){
		if(num!=0){
			x=(x-v+n)%n;
			position=position-this->decodebitnum[p];
			m=m-num;
		}
		else{
			m=m-1;
			x=(x-d+n)%n;
			position=position-bits;
		}
	}
	while(1){
		if(x>=pl){
			ans=m;
			break;
		}
		m++;
		if(m>r)
			break;
		decode(position,d);
		x=(x+d)%n;
	}
	return ans;
}

integer GAM::rightBoundary(integer b,integer l,integer r,integer pr){
	integer m=0;
	integer L=this->b;
	integer SL=this->a;
	integer x=samples->GetValue(b);
	
	integer ans=l-1;
	
	if(r>(b+1)*L-1)
		r=(b+1)*L-1;
	m=b*L;
	integer d=0;
	integer position = superoffset[m/SL]+offset->GetValue(m/L);
	while(m<l){
		decode(position,d);
		x=(x+d)%n;
		m++;
	}

	integer p=0;
	integer num=0;
	integer bits=0;
	bool loop=false;
	integer v=0;
	while(x<=pr && m<r){
		loop=true;
		p=this->getBits(position,16);
		num=this->decodevaluenum[p];
		if(num==0){
			m++;
			bits=this->decode(position,d);
			x=x+d;
		}
		else{
			m=m+num;
			position=position+this->decodebitnum[p];
			v=this->decoderesult[p];
			v=(v==0?256:v);
			x=(x+v)%n;
		}
	}
	if(loop){
		if(num!=0){
			x=(n+x-v)%n;
			position=position-this->decodebitnum[p];
			m=m-num;
		}
		else{
			m=m-1;
			x=(n+x-d)%n;
			position=position-bits;
		}
	}
	while(1){
		if(x>pr)
			break;
		ans=m;
		m++;
		if(m>r)
			break;
		decode(position,d);
		x=x+d;
	}

	return ans;
}
integer GAM::zeroRun(integer & position){
	integer y=0;
	integer D=16;
	integer x=getBits(position,D);
	integer w=y=zerostable[x];
	while(y==D){
		position=position+D;
		x=getBits(position,D);
		y=zerostable[x];
		w=w+y;
	}
	position=position+y;
	return w;
}

u64 GAM::getBits(integer position,integer num){
/*  u32 anchor=(position>>5);
	u64 temp1=sequence[anchor];
	u32 temp2=sequence[anchor+1];
	temp1=(temp1<<32)+temp2;
	integer overloop=((anchor+2)<<5)-position-num;
	return (temp1>>overloop)&((1<<num)-1);
*/
	integer anchor_start=position>>5;
	integer anchor_end=(position+num)>>5;
	if(anchor_end-anchor_start<2){
		u64 temp1=sequence[anchor_start];
		u32 temp2=sequence[anchor_start+1];
		temp1=(temp1<<32)+temp2;
		integer overloop=((anchor_start+2)<<5)-position-num;
		return (temp1>>overloop)&((1<<num)-1);
	}
	else{
		u64 temp1=sequence[anchor_start+0];
		u64 temp2=sequence[anchor_start+1];
		u64 temp3=sequence[anchor_start+2];
		i32 s1=(anchor_start+1)*32-index;
		i32 s2=num-32-s1;
		return ((temp1<<(32+s2))+(temp2<<s2)+(temp3>>(32-s2)))&((1ULL<<num)-1);
	}
}

integer GAM::blogsize(integer x){
	integer len=0;
	while(x>0){
		x=(x>>1);
		len++;
	}
	return len;
}

void GAM::initTables(){
	u8 * Rn=this->decodevaluenum=new u8[1<<16];
	u8 * Rb=this->decodebitnum=new u8[1<<16];
	u8 * Rx=this->decoderesult=new u8[1<<16];
	
	u32 tablesize=(1<<16);
	u32 B[4]={0xffffffff,0xffffffff,0xffffffff,0xffffffff};
	u32 *temp=this->sequence;
	this->sequence=B;
	integer b=0;
	integer num=0;
	integer x=0;
	integer d=0;
	integer preb=0;
	for(u32 i=0;i<tablesize;i++){
		B[0]=(i<<16);
		b=num=x=d=0;
		while(1){
			this->decode(b,d);
			if(b>16)
				break;
			x=x+d;
			num++;
			preb=b;
		}
		Rb[i]=preb;
		Rn[i]=num;
		Rx[i]=x;
	}
	this->sequence=temp;
}

