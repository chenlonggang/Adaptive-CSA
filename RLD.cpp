/*============================================
# Filename: RLD.cpp
# Ver 1.0 2014-11-17
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: run-length plus tuning delta coding for a gap sequence of Phi.
All of the coded value is gigger 1,so we can cut off the heading 0 of the original
delta coding
=============================================*/
#include"RLD.h"
RLD::RLD(integer *superoffset,InArray *offset,u32 *sequence,
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
	//this->index=index;
	initTables();
}

RLD::~RLD(){
	if(decodevaluenum)
		delete [] decodevaluenum;
	if(decodebitnum)
		delete [] decodebitnum;
	if(decoderesult)
		delete [] decoderesult;
}

void RLD::appendBinary(u64 y,integer valuewidth){
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

void RLD::encode(integer x){
	u64 y=x;
	integer size=blogsize(x);
	//Append_rlg(size);
	{
		u64 y=size;
		integer zeronums=blogsize(size)-2;
		index=index+zeronums;
		integer valuewidth=zeronums+2;
		appendBinary(y,valuewidth);
		
	/*	integer anchor=(index>>5);
		integer overloop =((anchor+2)<<5)-index-valuewidth;
		y=(y<<overloop);
		sequence[anchor]=(sequence[anchor]|(y>>32));
		sequence[anchor+1]=(sequence[anchor+1]|(y&0xffffffff));
		index=index+valuewidth;
	*/
	}
	integer valuewidth=size-1;
	y=(y^(1<<valuewidth));
	appendBinary(y,valuewidth);
	/*
	integer anchor=(index>>5);
	integer overloop =((anchor+2)<<5)-index-valuewidth;
	y=(y<<overloop);
	sequence[anchor]=(sequence[anchor]|(y>>32));
	sequence[anchor+1]=(sequence[anchor+1]|(y&0xffffffff));
	index=index+valuewidth;
	*/
}

integer RLD::decode(integer & position,integer &value){
	integer pos=position;
	integer x=this->zeroRun(position);
	integer v=getBits(position,x+2);
	position=position+x+2;
	value=(1<<(v-1))+getBits(position,v-1);
	position=position+v-1;
	return position-pos;
}

integer RLD::decode2(integer & position,integer &value){
	integer pos=position;
	integer x=this->zeroRun(position);
	integer v=getBits(position,x+2);
	position=position+x+2;

	if(v>32){
		position=(1<<30);
		return (1<<30);
	}
	value=(1<<(v-1))+getBits(position,v-1);
	position = position+v-1;
	return position-pos;
}

integer RLD::decodeAcc(integer position,integer base,integer num){
	integer i=0;
	integer value=0;
	while(i<num){
		decode(position,value);
		if(value%2==0){
			if(i+value/2>=num)
				return (base+num-i)%n;
			base=(base+value/2)%n;
			i=i+value/2;
		}
		else{
			base=(base+(value+1)/2)%n;
			i++;
		}								
	}
	return base;
}

integer RLD::leftBoundary(integer b,integer l,integer r,integer pl){
	integer m=0;
	integer ans=0;
	integer SL=this->a;
	integer L=this->b;
	integer x=samples->GetValue(b-1);
	if(r>b*L-1)
		r=b*L-1;
	ans=r+1;
	m=(b-1)*L;
	integer position=superoffset[m/SL]+offset->GetValue(b-1);
	integer d=0;

	while(m<l){
		this->decode(position,d);
		if(d%2==0){
			if(m+d/2>=l){
				x=(x+d/2-(m+d/2-l))%n;
				position=position-(m+d/2-l);
				m=l;
				break;
			}
			else{
				x=x+d/2;
				m=m+d/2;
			}
		}
		else{
			x=(x+(d+1)/2)%n;
			m++;
		}
	}
	integer p=0;
	integer num=0;
	integer bits=0;
	bool loop=false;
	while(x<pl && m<r){
		loop =true;
		p=this->getBits(position,16);
		num=this->decodevaluenum[p];
		if(num!=0){
			m=m+num;
			position=position+this->decodebitnum[p];
			x=(x+decoderesult[p])%n;
		}
		else{
			bits=this->decode(position,d);
			if(d%2==0){
				m=m+d/2;
				x=x+d/2;
			}
			else{
				m=m+1;
				x=x+(d+1)/2;
			}
		}
	}
	if(loop){
		if(num!=0){
			x=(n+x-this->decoderesult[p])%n;
			position=position-this->decodebitnum[p];
			m=m-num;
		}
		else{
			if(d%2==0){
				m=m-d/2;
				x=(n+x-d/2)%n;
			}
			else{
				m=m-1;
				x=(n+x-(d+1)/2)%n;
			}
			position =position-bits;
		}
	}

	integer run=0;
	while(1){
		if(m>=l && x>=pl){
			ans=m;
			break;
		}
		m++;
		if(m>r)
			break;
		if(run>0){
			x=(x+1)%n;
			run--;
		}
		else{
			decode(position,d);
			if(d%2==0){
				run=d/2-1;
				x=(x+1)%n;
			}
			else
				x=(x+(d+1)/2)%n;
		}
	}

	return ans;
}

integer RLD::rightBoundary(integer b,integer l,integer r,integer pr){
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

	integer p=0;
	integer num=0;
	integer bits=0;
	bool loop=false;
	while(x<=pr && m<r){
		loop=true;
		p=this->getBits(position,16);
		num=this->decodevaluenum[p];
		if(num==0){
			bits=decode(position,d);
			if(d%2==0){
				m=m+d/2;
				x=(x+d/2)%n;
			}
			else{
				m++;
				x=(x+(d+1)/2)%n;
			}
		}
		else{
			m=m+num;
			position=position+this->decodebitnum[p];
			x=(x+this->decoderesult[p])%n;
		}
	}
	if(loop){
		if(num!=0){
			x=(n+x-this->decoderesult[p])%n;
			position=position-this->decodebitnum[p];
			m=m-num;
		}
		else{
			if(d%2==0){
				m=m-d/2;
				x=(x-d/2+n)%n;
			}
			else{
				m=m-1;
				x=(n+x-(d+1)/2)%n;
			}
			position=position-bits;
		}
	}
	integer run=0;
	while(1){
		if(m>=l && x>pr)
			break;
		ans=m;
		m++;
		if(m>r)
			break;
		if(run>0){
			x=(x+1)%n;
			run--;
		}
		else{
			decode(position,d);
			if(d%2==0){
				run=d/2-1;
				x=(x+1)%n;
			}
			else
				x=(x+(d+1)/2)%n;
		}
	}

	return ans;
}

void RLD::initTables(){
	u8 * Rn=this->decodevaluenum=new u8[1<<16];
	u8 * Rb=this->decodebitnum=new u8[1<<16];
	u16 * Rx=this->decoderesult=new u16[1<<16];
	u32 tablesize=(1<<16);
	u32 B[4]={0xffffffff,0xffffffff,0xffffffff,0xffffffff};
	u32 *temp=this->sequence;
	this->sequence=B;
	integer b=0;
	integer d=0;
	integer num=0;
	integer preb=0;
	integer x=0;
	for(u32 i=0;i<tablesize;i++){
		B[0]=(i<<16);
		b=num=x=d=0;
		while(1){
			this->decode2(b,d);
			if(b>16)
				break;
			if(d%2==0){
				x=x+d/2;
				num=num+d/2;
			}
			else{
				x=x+(d+1)/2;
				num++;
			}
			preb=b;
		}
		Rn[i]=num;
		Rx[i]=x;
		Rb[i]=preb;
	}
	this->sequence=temp;
}

u64 RLD::getBits(integer position,integer num){
/*	u32 anchor=(position>>5);
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

integer RLD::blogsize(integer x){
	integer len=0;
	while(x>0){
		x=(x>>1);
		len++;
	}
	return len;
}

integer RLD::zeroRun(integer & position){
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
