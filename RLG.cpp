/*============================================
# Filename: RLG.cpp
# Ver 1.0 2014-11-17
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description:run-length plus tuning gamma coding for a gap sequence of Phi.
All of the coded value is bigger than 1, so we can cut off the heading 0 of 
the original gamma coding
=============================================*/
#include"RLG.h"
RLG::RLG(integer *superoffset,InArray *offset,u32 *sequence,
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

RLG::~RLG(){
	if(decodevaluenum)
		delete [] decodevaluenum;
	if(decodebitnum)
		delete [] decodebitnum;
	if(decoderesult)
		delete [] decoderesult;
}

void RLG::encode(integer x){
	u64 y=x;
	integer zeronums=blogsize(x)-2;
	index=index+zeronums;
	integer valuewidth=zeronums+2;
	integer anchor=(index>>5);
	integer overloop =((anchor+2)<<5)-index-valuewidth;
	y=(y<<overloop);
	sequence[anchor]=(sequence[anchor]|(y>>32));
	sequence[anchor+1]=(sequence[anchor+1]|(y&0xffffffff));
	index=index+valuewidth;
}

integer RLG::decode(integer & position,integer &value){
	integer a=this->zeroRun(position);
	value=getBits(position,a+2);
	position=position+a+2;
	return 2*a+2;
}

integer RLG::decodeAcc(integer position,integer base,integer num){
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

integer RLG::leftBoundary(integer b,integer l,integer r,integer pl){
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
	integer v=0;
	while(x<pl && m<r){
		loop =true;
		p=this->getBits(position,16);
		num=this->decodevaluenum[p];
		if(num!=0){
			m=m+num;
			position=position+this->decodebitnum[p];
			v=decoderesult[p];
			v=(v==0?256:v);
			x=(x+v)%n;
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
			x=(n+x-v)%n;
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

integer RLG::rightBoundary(integer b,integer l,integer r,integer pr){
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
	integer v=0;
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
			v=decoderesult[p];
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

integer RLG::getBits(integer position,integer num){
	u32 anchor=(position>>5);
	u64 temp1=sequence[anchor];
	u32 temp2=sequence[anchor+1];
	temp1=(temp1<<32)+temp2;
	integer overloop=((anchor+2)<<5)-position-num;
	return (temp1>>overloop)&((1<<num)-1);
}

integer RLG::blogsize(integer x){
	integer len=0;
	while(x>0){
		x=(x>>1);
		len++;
	}
	return len;
}

integer RLG::zeroRun(integer & position){
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

void RLG::initTables(){
	u8 * Rn=this->decodevaluenum=new u8[1<<16];
	u8 * Rb=this->decodebitnum=new u8[1<<16];
	u8 * Rx=this->decoderesult=new u8[1<<16];
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
			this->decode(b,d);
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
//		if(x>255)
//			cout<<"rlg x,num,b "<<x<<" "<<num<<" "<<preb<<endl;
		Rb[i]=preb;
	}
	this->sequence=temp;
}

