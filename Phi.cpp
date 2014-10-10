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

	//MethodsStatic();
	//cout<<"code is "<<(checkCodeAndFill_getPhiArray()==1?"right":"wrong")<<endl;
	//cout<<"getValue is "<<(checkgetValue()==1?"right":"wrong")<<endl;
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
/*
i32 Phi::getValue(const i32 index){
	return value[index];
}
*/
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
				if(runs==b-1){//ALL1
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
		
		switch(method){
			case 0:len1=len1+2*blogsize(gap)-1;Append(gap);break;
			case 1:
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
				   };break;
			case 2:len1=len1+0;break;
			default:cerr<<"method error"<<endl;break;
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

i32 Phi::decodeGamma(i32 & position,i32 & value){
	i32 a=this->zeroRun(position);
	value=getBits(position,a+1);
	position=position+a+1;
	return 2*a+1;
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
	//i32 soffset=0;
	//i32 offset=0;
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
			case 1:decodeGamma(position,value);
				   if(value%2==0){
					   i32 num=value/2;
					   for(i32 j=0;j<num;j++){
						   base=(base+1)%n;
						   phiarray[i+j]=base;
					   }
					   i=i+num-1;
				   }
				   else{
					   value=(value+3)/2;
					   base=(base+value)%n;
					   phiarray[i]=base;;
				   };
				   break;
			case 2:i32 ones=b-1;
				   for(i32 j=0;j<ones;j++){
					   base=(base+1)%n;
					   phiarray[i+j]=base;
				   };
				   i=i+ones-1;
				   break;
			//default:cerr<<"method error"<<endl;break;
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
		decodeGamma(position,value);
		if(value%2==0){
			if(i+value/2>=num)
				return (base+num-i)%n;
			base=(base+value/2)%n;
			i=i+value/2;
		}
		else{
			base=(base+(value+3)/2)%n;
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
	//return 0;
	//i32 ans=0;
	//i32 SL=a;
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
	//return 0;
	i32 m=0;
	i32 run=0;
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
	return ans;
}

i32 Phi::leftBoundary_all1(i32 b,i32 l,i32 r,i32 pl){
	//return 0;
	i32 m=0;
	i32 ans=0;
	i32 L=this->b;
	if(r>b*L-1)
		r=b*L-1;
	ans=r+1;
	i32 x=samples->GetValue(b-1);
	m=(b-1)*L;
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
	//if(b==0)
	//	return 0;
	i32 method=methods->GetValue(b);
	switch(method){
		case 0:return rightBoundary_gamma(b,l,r,pr);
		case 1:return rightBoundary_rlg(b,l,r,pr);
		case 2:return rightBoundary_all1(b,l,r,pr);
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
	return ans;

}



