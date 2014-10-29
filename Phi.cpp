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
//#define De
Phi::Phi(i32 * phiarray,i32 n,i32 bs){
	this->n=n;
	this->b=bs;
	this->a=18*b;
	this->index=0;
	value=phiarray;
	methodsAndSpace();
	allocAndInit();
	initTables();
	codeAndFill();

//	MethodsStatic();
//	cout<<"code is "<<(checkCodeAndFill_getPhiArray()==1?"right":"wrong")<<endl;
//	cout<<"getValue is "<<(checkgetValue()==1?"right":"wrong")<<endl;
	
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
	if(decodevaluenum_gam) delete [] decodevaluenum_gam;
	if(decodebitnum_gam)   delete [] decodebitnum_gam;
	if(decoderesult_gam)   delete [] decoderesult_gam;
	if(decodevaluenum_rlg) delete [] decodevaluenum_rlg;
	if(decodebitnum_rlg)   delete [] decodebitnum_rlg;
	if(decoderesult_rlg)   delete [] decoderesult_rlg;
	if(decodevaluenum_rld) delete [] decodevaluenum_rld;
	if(decodebitnum_rld)   delete [] decodebitnum_rld;
	if(decoderesult_rld)   delete [] decoderesult_rld;
	
	samples=offset=NULL;
	superoffset=NULL;
	sequence=NULL;
	zerostable=NULL;
	decodevaluenum_gam=decodebitnum_gam=decoderesult_gam=NULL;
	decodevaluenum_rlg=decodebitnum_rlg=decoderesult_rlg=NULL;
	decodevaluenum_rld=decodebitnum_rld=decoderesult_rld=NULL;

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
	//s.loadi32(alphabetsize);
	//zeroatable
	this->zerostable=new u16[1<<16];
	this->initTables();
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
	return 0;
}

i32 Phi::deltasize(i32 v){
	//i32 x=blogsize(v)-1;
	//i32 gammasize=2*blogsize(x+1)-1;
	//return gammasize+x-1;
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
			
			gap=value[j]-pre;
			if(gap<0)
				gap=gap+n;
			//cout<<gap<<endl;
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
			
			if((j+1)%b==0 || (j+1)==n){
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
		if(len>maxlen)
			maxlen=len;
		totlen=totlen+len;
		len=0;
	}
	lenofsequence=totlen/32+1;
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
	u16 * Rn_gam=this->decodevaluenum_gam;
	u16 * Rb_gam=this->decodebitnum_gam;
	u16 * Rx_gam=this->decoderesult_gam;
	u16 * Rn_rlg=this->decodevaluenum_rlg;
	u16 * Rb_rlg=this->decodebitnum_rlg;
	u16 * Rx_rlg=this->decoderesult_rlg;
	u16 * Rn_rld=this->decodevaluenum_rld;
	u16 * Rb_rld=this->decodebitnum_rld;
	u16 * Rx_rld=this->decoderesult_rld;
	u32 tablesize=(1<<16);
	u32 B[4]={0xffffffff,0xffffffff,0xffffffff,0xffffffff};
	u32 *temp=this->sequence;
	this->sequence=B;
	i32 b=0;
	i32 num=0;
	i32 x=0;
	i32 d=0;
	i32 preb=0;
	for(u32 i=0;i<tablesize;i++){
		B[0]=(i<<16);
		b=num=x=d=0;
		while(1){
			this->decodeGamma(b,d);
			if(b>16)
				break;
			x=x+d;
			num++;
			preb=b;
		}
		Rb_gam[i]=preb;
		Rn_gam[i]=num;
		Rx_gam[i]=x;
		
		b=d=num=x=0;
		preb=0;
		while(1){
			this->decodeGamma_rlg(b,d);
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
		Rn_rlg[i]=num;
		Rx_rlg[i]=x;
		Rb_rlg[i]=preb;
		
		b=d=num=x=0;
		preb=0;
		while(1){
			this->decodeDelta_rld2(b,d);
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
		Rn_rld[i]=num;
		Rx_rld[i]=x;
		Rb_rld[i]=preb;
//		printf("%x\n",B[0]);
				
	}
	this->sequence=temp;

}
	
void Phi::allocAndInit(){
	this->superoffset=new i32[lenofsuperoffset];
	this->offset=new InArray(n/b+1,blogsize(maxsbs));
	this->samples=new InArray(n/b+1,blogsize(n));
	this->sequence=new u32[lenofsequence];
	
	this->zerostable = new u16[1<<16];
	this->decodevaluenum_gam=new u16[1<<16];
	this->decodebitnum_gam=new u16[1<<16];
	this->decoderesult_gam =new u16[1<<16];
	this->decodevaluenum_rlg=new u16[1<<16];
	this->decodebitnum_rlg=new u16[1<<16];
	this->decoderesult_rlg=new u16[1<<16];
	this->decodevaluenum_rld=new u16[1<<16];
	this->decodebitnum_rld=new u16[1<<16];
	this->decoderesult_rld=new u16[1<<16];
	
	memset(zerostable,0,sizeof(u16)*(1<<16));
	memset(decodevaluenum_gam,0,sizeof(u16)*(1<<16));
	memset(decodebitnum_gam,0,sizeof(u16)*(1<<16));
	memset(decoderesult_gam,0,sizeof(u16)*(1<<16));
	memset(decodevaluenum_rlg,0,sizeof(u16)*(1<<16));
	memset(decodebitnum_rlg,0,sizeof(u16)*(1<<16));
	memset(decoderesult_rlg,0,sizeof(u16)*(1<<16));
	memset(decodevaluenum_rld,0,sizeof(u16)*(1<<16));
	memset(decodebitnum_rld,0,sizeof(u16)*(1<<16));
	memset(decoderesult_rld,0,sizeof(u16)*(1<<16));
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
			case 0:len1=len1+2*blogsize(gap)-1;Append(gap);break;
			case 1:
				   if(gap==1){
					   runs++;
					   if((i+1)%b==0){
						   len1=len1+2*blogsize(2*runs)-2;
						   Append_rlg(2*runs);
						   runs=0;
					   }
				   }
				   else{
					   if(runs!=0){
						   len1=len1+2*blogsize(2*runs)-2;
						   Append_rlg(2*runs);
					   }
					   len1=len1+2*blogsize(2*gap-1)-2;
					   Append_rlg(2*gap-1);
					   runs=0;
				   };break;
			case 2:len1=len1+0;break;
			case 3:
				   if(gap==1){
					   runs++;
				       if((i+1)%b==0){
						   len1=len1+deltasize(2*runs);
						   Append_rld(2*runs);
						   runs=0;
					   }
				   }
				   else{
					   if(runs!=0){
						   len1=len1+deltasize(2*runs);
						   Append_rld(2*runs);
					   }
					   len1=len1+deltasize(2*gap-1);
					   Append_rld(2*gap-1);
					   runs=0;
				   };break;
			default:cerr<<"347: method error"<<endl;exit(0);
		}
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

void Phi::Append_rlg(i32 x){
	u64 y=x;
	i32 zeronums=blogsize(x)-2;
	index=index+zeronums;
	i32 valuewidth=zeronums+2;

	i32 anchor=(index>>5);
	i32 overloop =((anchor+2)<<5)-index-valuewidth;
	y=(y<<overloop);
	sequence[anchor]=(sequence[anchor]|(y>>32));
	sequence[anchor+1]=(sequence[anchor+1]|(y&0xffffffff));
	index=index+valuewidth;
}

void Phi::Append_rld(i32 x){
	u64 y=x;
	i32 size=blogsize(x);
	Append_rlg(size);
	i32 valuewidth=size-1;
	y=(y^(1<<valuewidth));

	i32 anchor=(index>>5);
	i32 overloop =((anchor+2)<<5)-index-valuewidth;
	y=(y<<overloop);
	sequence[anchor]=(sequence[anchor]|(y>>32));
	sequence[anchor+1]=(sequence[anchor+1]|(y&0xffffffff));
	index=index+valuewidth;
}


i32 Phi::decodeGamma(i32 & position,i32 & value){
	i32 a=this->zeroRun(position);
	value=getBits(position,a+1);
	position=position+a+1;
	return 2*a+1;
}

i32 Phi::decodeGamma_rlg(i32 & position,i32 & value){
	i32 a=this->zeroRun(position);
	//cout<<a+2<<endl;
	value=getBits(position,a+2);
	position=position+a+2;
	return 2*a+2;
}

i32 Phi::decodeDelta_rld2(i32 & position,i32 & value){
	i32 pos=position;
	decodeGamma_rlg(position,value);
	i32 v=value;
	if(value>32){
		position=(1<<30);
		return (1<<30);
	}
	value=(1<<(v-1))+getBits(position,v-1);
	position = position+v-1;
	return position-pos;
}

i32 Phi::decodeDelta_rld(i32 & position,i32 & value){
	i32 pos=position;
	decodeGamma_rlg(position,value);
	i32 v=value;
	value=(1<<(v-1))+getBits(position,v-1);
	position=position+v-1;
	return position-pos;
}

i32 Phi::zeroRun(i32 & position){
	i32 y=0;
	i32 D=16;
	i32 x=getBits(position,D);
	i32 w=y=zerostable[x];
	while(y==D){
		position=position+D;
		x=getBits(position,D);
		y=zerostable[x];
		w=w+y;
	}
	position=position+y;
	return w;
}

i32 Phi::getBits(i32 position,i32 num){
//	cout<<"bits "<<num<<endl;
	u32 anchor=(position>>5);
	u64 temp1=sequence[anchor];
	u32 temp2=sequence[anchor+1];
	temp1=(temp1<<32)+temp2;
	i32 overloop=((anchor+2)<<5)-position-num;
	return (temp1>>overloop)&((1<<num)-1);
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
			case 0:decodeGamma(position,value);
				   base=(base+value)%n;
				   phiarray[i]=base;
				   break;
			case 1:decodeGamma_rlg(position,value);
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
				   decodeDelta_rld(position,value);
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
		case 0:return gammaSequence(position,base,overloop);
		case 1:return rlgSequence(position,base,overloop);
		case 2:return all1Sequence(position,base,overloop);
		case 3:return rldSequence(position,base,overloop);
		default:cerr<<"method error"<<endl;exit(0);
	}
}

i32 Phi::gammaSequence(i32 position,i32 base,i32 num){
	i32 i=0;
	i32 value=0;
	while(i<num){
		decodeGamma(position,value);
		base=(base+value)%n;
		i++;
	}
	return base;
}

i32 Phi::rlgSequence(i32 position,i32 base,i32 num){
	i32 i=0;
	i32 value=0;
	while(i<num){
		decodeGamma_rlg(position,value);
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

i32 Phi::rldSequence(i32 position,i32 base,i32 num){
	i32 i=0;
	i32 value=0;
	while(i<num){
		decodeDelta_rld(position,value);
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

i32 Phi::all1Sequence(i32 position,i32 base,i32 num){
	return (base+num)%n;
}

/*在区间[L,R]内，找到第一个Phi值大于等于l的
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
		case 0:return leftBoundary_gamma(b,l,r,pl);
		case 1:return leftBoundary_rlg(b,l,r,pl);
		case 2:return leftBoundary_all1(b,l,r,pl);
		case 3:return leftBoundary_rld(b,l,r,pl);
		default :cerr<<"method erroe"<<endl;exit(0);
	}
}

i32 Phi::leftBoundary_gamma(i32 b,i32 l,i32 r,i32 pl){
	i32 ans=0;
	i32 m=0;
	i32 SL=a;
	i32 L=this->b;
	i32 x=samples->GetValue(b-1);
	if(r>b*L-1)
		r=b*L-1;
	ans=r+1;
	m=(b-1)*L;
	i32 position=superoffset[m/SL]+offset->GetValue(b-1);
	i32 d=0;
	while(m<l){
		decodeGamma(position,d);
		x=(x+d)%n;
		m++;
	}


/*
	while(1){
		if(x>=pl){
			ans=m;
			break;
		}
		m++;
		if(m>r)
			break;
		decodeGamma(position,d);
		x=(x+d)%n;
	}
*/
	i32 p=0;
	i32 num=0;
	i32 bits=0;
	bool loop=false;
	while(x<pl && m<r){
		loop=true;
		p=getBits(position,16);
		num=this->decodevaluenum_gam[p];
		if(num!=0){
			m=m+num;
			position=position+this->decodebitnum_gam[p];
			x=(x+this->decoderesult_gam[p])%n;
		}
		else{
			m++;
			bits=this->decodeGamma(position,d);
			x=x+d;
		}
	}
	if(loop){
		if(num!=0){
			x=(x-this->decoderesult_gam[p]+n)%n;
			position=position-this->decodebitnum_gam[p];
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
		decodeGamma(position,d);
		x=(x+d)%n;
	}
	return ans;
}

i32 Phi::leftBoundary_rlg(i32 b,i32 l,i32 r,i32 pl){
	i32 m=0;
	i32 ans=0;
	i32 SL=this->a;
	i32 L=this->b;
	i32 x=samples->GetValue(b-1);
	if(r>b*L-1)
		r=b*L-1;
	ans=r+1;
	m=(b-1)*L;
	i32 position=superoffset[m/SL]+offset->GetValue(b-1);
	i32 d=0;
/*
	i32 run=0;
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
			decodeGamma(position,d);
			if(d%2==0){
				run=d/2-1;
				x=(x+1)%n;
			}
			else
				x=(x+(d+3)/2)%n;
		}
	}
*/
	
	while(m<l){
		this->decodeGamma_rlg(position,d);
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
	i32 p=0;
	i32 num=0;
	i32 bits=0;
	bool loop=false;
	while(x<pl && m<r){
		loop =true;
		p=this->getBits(position,16);
		num=this->decodevaluenum_rlg[p];
		if(num!=0){
			m=m+num;
			position=position+this->decodebitnum_rlg[p];
			x=(x+decoderesult_rlg[p])%n;
		}
		else{
			bits=this->decodeGamma_rlg(position,d);
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
			x=(n+x-this->decoderesult_rlg[p])%n;
			position=position-this->decodebitnum_rlg[p];
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

	i32 run=0;
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
			decodeGamma_rlg(position,d);
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

i32 Phi::leftBoundary_rld(i32 b,i32 l,i32 r,i32 pl){
	i32 m=0;
	i32 ans=0;
	i32 SL=this->a;
	i32 L=this->b;
	i32 x=samples->GetValue(b-1);
	if(r>b*L-1)
		r=b*L-1;
	ans=r+1;
	m=(b-1)*L;
	i32 position=superoffset[m/SL]+offset->GetValue(b-1);
	i32 d=0;
/*
	i32 run=0;
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
			decodeGamma(position,d);
			if(d%2==0){
				run=d/2-1;
				x=(x+1)%n;
			}
			else
				x=(x+(d+3)/2)%n;
		}
	}
*/
	
	while(m<l){
		this->decodeDelta_rld(position,d);
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
	i32 p=0;
	i32 num=0;
	i32 bits=0;
	bool loop=false;
	while(x<pl && m<r){
		loop =true;
		p=this->getBits(position,16);
		num=this->decodevaluenum_rld[p];
		if(num!=0){
			m=m+num;
			position=position+this->decodebitnum_rld[p];
			x=(x+decoderesult_rld[p])%n;
		}
		else{
			bits=this->decodeDelta_rld(position,d);
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
			x=(n+x-this->decoderesult_rld[p])%n;
			position=position-this->decodebitnum_rld[p];
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

	i32 run=0;
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
			decodeDelta_rld(position,d);
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

i32 Phi::leftBoundary_all1(i32 b,i32 l,i32 r,i32 pl){
	i32 m=0;
	i32 ans=0;
	i32 L=this->b;
	if(r>b*L-1)
		r=b*L-1;
	ans=r+1;
	i32 x=samples->GetValue(b-1);
	m=(b-1)*L;
/*
	i32 run=L-1;
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
	}
*/

	if(l>m){
		i32 step=l-m;
		x=(x+l-m+n)%n;
		m=l;
		if(pl<=(x+L-1-step+n)%n)
			ans=m+(pl-x+n)%n;
	}
	else{
		if(pl<=(x+L-1)%n)
			ans=m+pl-x;
	}
	return ans;
}
/*在区间[L,R]内，找到最后一个Phi值小于等于l的
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
		case 0:return rightBoundary_gamma(b,l,r,pr);
		case 1:return rightBoundary_rlg(b,l,r,pr);
		case 2:return rightBoundary_all1(b,l,r,pr);
		case 3:return rightBoundary_rld(b,l,r,pr);
		default:cerr<<"method error"<<endl;exit(0);
	}
}

i32 Phi::rightBoundary_gamma(i32 b,i32 l,i32 r,i32 pr){
	i32 m=0;
	i32 L=this->b;
	i32 SL=this->a;
	i32 x=samples->GetValue(b);
	
	i32 ans=l-1;
	
	if(r>(b+1)*L-1)
		r=(b+1)*L-1;
	m=b*L;
	i32 d=0;
	i32 position = superoffset[m/SL]+offset->GetValue(m/L);
	while(m<l){
		decodeGamma(position,d);
		x=(x+d)%n;
		m++;
	}

/*
	while(1){
		if(x>pr)
			break;
		ans=m;
		m++;
		if(m>r)
			break;
		decodeGamma(position,d);
		x=(x+d)%n;
	}
*/

	i32 p=0;
	i32 num=0;
	i32 bits=0;
	bool loop=false;
	while(x<=pr && m<r){
		loop=true;
		p=this->getBits(position,16);
		num=this->decodevaluenum_gam[p];
		if(num==0){
			m++;
			bits=this->decodeGamma(position,d);
			x=x+d;
		}
		else{
			m=m+num;
			position=position+this->decodebitnum_gam[p];
			x=(x+this->decoderesult_gam[p])%n;
		}
	}
	if(loop){
		if(num!=0){
			x=(n+x-this->decoderesult_gam[p])%n;
			position=position-this->decodebitnum_gam[p];
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
		decodeGamma(position,d);
		x=x+d;
	}

	return ans;
}

i32 Phi::rightBoundary_rlg(i32 b,i32 l,i32 r,i32 pr){
	i32 m=0;
	i32 L=this->b;
	i32 SL=this->a;
	i32 x=samples->GetValue(b);

	i32 ans=l-1;
	if(r>(b+1)*L-1)
		r=(b+1)*L-1;
	m=b*L;
	i32 d=0;
	i32 position = superoffset[m/SL]+offset->GetValue(m/L);
/*	
	i32 run=0;
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
			decodeGamma(position,d);
			if(d%2==0){
				run=d/2-1;
				x=(x+1)%n;
			}
			else
				x=(x+(d+3)/2)%n;
		}
	}

*/
	i32 p=0;
	i32 num=0;
	i32 bits=0;
	bool loop=false;
	while(x<=pr && m<r){
		loop=true;
		p=this->getBits(position,16);
		num=this->decodevaluenum_rlg[p];
		if(num==0){
			bits=decodeGamma_rlg(position,d);
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
			position=position+this->decodebitnum_rlg[p];
			x=(x+this->decoderesult_rlg[p])%n;
		}
	}
	if(loop){
		if(num!=0){
			x=(n+x-this->decoderesult_rlg[p])%n;
			position=position-this->decodebitnum_rlg[p];
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
	i32 run=0;
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
			decodeGamma_rlg(position,d);
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
i32 Phi::rightBoundary_rld(i32 b,i32 l,i32 r,i32 pr){
	i32 m=0;
	i32 L=this->b;
	i32 SL=this->a;
	i32 x=samples->GetValue(b);

	i32 ans=l-1;
	if(r>(b+1)*L-1)
		r=(b+1)*L-1;
	m=b*L;
	i32 d=0;
	i32 position = superoffset[m/SL]+offset->GetValue(m/L);
/*	
	i32 run=0;
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
			decodeGamma(position,d);
			if(d%2==0){
				run=d/2-1;
				x=(x+1)%n;
			}
			else
				x=(x+(d+3)/2)%n;
		}
	}

*/
	i32 p=0;
	i32 num=0;
	i32 bits=0;
	bool loop=false;
	while(x<=pr && m<r){
		loop=true;
		p=this->getBits(position,16);
		num=this->decodevaluenum_rld[p];
		if(num==0){
			bits=decodeDelta_rld(position,d);
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
			position=position+this->decodebitnum_rld[p];
			x=(x+this->decoderesult_rld[p])%n;
		}
	}
	if(loop){
		if(num!=0){
			x=(n+x-this->decoderesult_rld[p])%n;
			position=position-this->decodebitnum_rld[p];
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
	i32 run=0;
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
			decodeDelta_rld(position,d);
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


i32 Phi::rightBoundary_all1(i32 b,i32 l,i32 r,i32 pr){
	i32 L=this->b;
	i32 m=0;
	i32 ans=l-1;
	if(r>(b+1)*L-1)
		r=(b+1)*L-1;

	i32 x=samples->GetValue(b);
	m=b*L;
/*
	i32 run=L-1;
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
	}
*/

	
	if(pr<=(x+r-m+n)%n)
		ans=m+(pr-x)%n;
	else
		ans=r;

	return ans;

}

