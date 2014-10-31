/*============================================
# Filename: CSA_Handler.cpp
# Ver 1.0 2014-10-06
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
=============================================*/
#include"ds_ssort.h"
#include"CSA_Handler.h"
#include<string.h>
CSA_Handler::CSA_Handler(const char * sourcefile,i32 L,i32 D,i32 speedlevel):u(){
	if(speedlevel<0)
		speedlevel=0;
	if(speedlevel>2)
		speedlevel=2;
	this->speedlevel=speedlevel;
	this->SL=L*18;
	this->L=L;
	this->D=D;
	this->RD=D*16;

	this->code = new i32[256];
	memset(code,0,256*sizeof(i32));

	this->alphabetsize=0;
	uchar * T=NULL;
	//读取文件
	T=getFile(sourcefile);
	//统计，初始化code/incode/start表
	statics(T);
	i32 * SA = new i32[n];
	//计算后缀数组 
	ds_ssort(T,SA,n);
	//计算Phi数组
	i32 * phiarray = phiArray(SA,T);
	
	delete [] T;//T is useless now
	T=NULL;
	//计算gap中1的比例，依次决定参数 
	//ratio暂时不决定D和RD，只决定L和SL.
	computerPar(phiarray);
	//采样SAL和RankL
	sampleSAAndRank(SA);
	//from now on ,SA is useless;
	delete [] SA;
	SA=NULL;
	//创建Phi结构。参数：
	//phiArray:Phi数组
	//n:Phi数组长度
	//L:块大小，超快大小固定位块大小的18倍,不用传.
	phi = new Phi(phiarray,n,this->L);
	
	delete [] phiarray;
	phiarray=NULL;

}

CSA_Handler::CSA_Handler(const CSA_Handler & h):u(h.u){
	assignDataMembers(h);
}

CSA_Handler & CSA_Handler::operator =(const CSA_Handler &h){
	if(u.reattach(h.u))
		deletePointers();
	assignDataMembers(h);
	return *this;
}

CSA_Handler::~CSA_Handler(){
	if(u.only())
		deletePointers();
}

void CSA_Handler::assignDataMembers(const CSA_Handler & h){
	SL=h.SL;
	L=h.L;
	D=h.D;
	RD=h.RD;
	SAL=h.SAL;
	RankL=h.RankL;
	n=h.n;
	code=h.code;
	incode=h.incode;
	start=h.start;
	alphabetsize=h.alphabetsize;
	lastchar=h.lastchar;
	phi=h.phi;
	speedlevel=h.speedlevel;
}

void CSA_Handler::deletePointers(){
	if(SAL)	  delete SAL;
	if(RankL) delete RankL;
	if(phi)   delete phi;
	if(code)  delete [] code;
	if(incode)delete [] incode;
	if(start) delete [] start;
	SAL=RankL=NULL;
	phi=NULL;
	code=incode=start=NULL;
}

void CSA_Handler::computerPar(i32 * phi){
	//计算gap中1的比例，依次决定参数 
	i32 pre=0;
	i32 gap=0;
	i32 num=0;
	for(i32 i=0;i<n;i++)
	{
		gap=phi[i]-pre;
		if(gap==1)
			num++;
		pre=phi[i];
	}
	double ratio = (num*1.0)/n;
	i32 multi=1;
	double ratio1=0.0;
	double ratio2=0.0;
	switch(speedlevel){
		case 0:ratio1=0.50;ratio2=0.60;break;
		case 1:ratio1=0.60;ratio2=0.75;break;
		case 2:ratio1=0.65;ratio2=0.80;break;
	}
	if(ratio<ratio1)
		multi=1;
	else if(ratio<ratio2)
		multi=2;
	else 
		multi=4;
	//multi=1;
	this->L=this->L*multi;
	this->SL=this->L*18;
  cout<<"	CSA_Handler--136: multi:  "<<multi<<endl;
//	this->D=this->D*multi;
//	this->RD=this->D*16;
}

void CSA_Handler::sampleSAAndRank(i32 * SA){
	i32 i=0;
	i32 j=0;
	i32 step1=D;
	i32 step2=RD;
	SAL = new InArray(n/step1+1,blog(n));
	RankL=new InArray(n/step2+1,blog(n));
	for(i=0,j=0;i<n;i=i+step1,j++)
		SAL->SetValue(j,SA[i]);
	for(i=0;i<n;i++)
		if(SA[i]%step2==0)
			RankL->SetValue(SA[i]/step2,i);
}


i32 * CSA_Handler::phiArray(i32 *SA,uchar * T){
	//计算Phi数组
	i32 * phi = new i32[n];
	memset(phi,0,n*sizeof(i32));
	i32 * temp = new i32[alphabetsize+1];
	for(i32 i=0;i<alphabetsize+1;i++)
		temp[i]=this->start[i];
	i32 index = temp[code[lastchar]];
	temp[code[lastchar]]++;
	i32 h=0;
	uchar c=0;
	i32 pos=0;
	for(i32 i=0;i<n;i++){
		pos=SA[i];
		if(pos==0){
			h=i;
			continue;
		}
		c=T[pos-1];
		phi[temp[code[c]]++]=i;
	}
	phi[index]=h;
	delete [] temp;
	temp=NULL;
	return phi;
}



uchar * CSA_Handler::getFile(const char * filename){
	FILE * fp=fopen(filename,"r+");
	if(fp==NULL){
		cerr<<"Be sure that file is available"<<endl;
		exit(0);
	}
	// 得到文件大小，最后再把文件指针指向文件开始的地方
	fseek(fp,0,SEEK_END);
	this->n=ftell(fp);
	fseek(fp,0,SEEK_SET);

	i32 overshot =init_ds_ssort(500,2000);
	uchar * T=new uchar[n+overshot];

	i32 e=0;
	i32 num=0;

	while((e=fread(T+num,sizeof(uchar),n-num,fp))!=0)
		num=num+e;
	if(num!=n){
		cerr<<"broken while reading file"<<endl;
		exit(0);
	}

	fclose(fp);
	return T;
}

void CSA_Handler::statics(uchar *T){
	for(i32 i=0;i<n;i++)
		code[T[i]]++;

	for(i32 i=0;i<256;i++)
		if(code[i]!=0)
			alphabetsize++;
	this->start=new i32[alphabetsize+1];
	this->start[alphabetsize]=n;
	this->start[0]=0;
	i32 k=1;
	i32 pre=0;

	for(i32 i=0;i<256;i++)
		if(code[i]!=0){
			start[k]=pre+code[i];
			pre=start[k];
			k++;
		}

	this->incode = new i32[alphabetsize];
	k=0;
	for(i32 i=0;i<256;i++)
		if(code[i]!=0){
			code[i]=k;
			incode[k]=i;
			k++;
		}
		else
			code[i]=-1;
	lastchar=T[n-1];
}

void CSA_Handler::Counting(const char * pattern,i32 &num){
	i32 L=0;
	i32 R=0;
	countSearch2(pattern,L,R);
	num=R-L+1;
}

i32 CSA_Handler::lookUp(i32 i){
	i32 step=0;
	while(i%D!=0){
		i=phi->getValue(i);
		step++;
	}
	i=i/D;
	return (n+SAL->GetValue(i)-step)%n;
}

void CSA_Handler::Locating(const char * pattern,i32 &num,i32 *&pos){
	i32 L=0;
	i32 R=0;
	this->countSearch2(pattern,L,R);
	num=R-L+1;
	if(L>R)
		return ;
	pos=new i32[num];
	for(i32 i=L;i<=R;i++)
		pos[i-L]=lookUp(i);
}

i32 CSA_Handler::inverse(i32 pos){
	i32 anchor = pos/RD;
	i32 p=anchor*RD;
	i32 sa=RankL->GetValue(anchor);
	while(p<pos){
		sa=phi->getValue(sa);
		p++;
	}
	return sa;
}

i32 CSA_Handler::phiList(i32 i){
	i32 l=0;
	i32 r=alphabetsize;
	i32 m=0;
	while(l<r){
		m=(l+r)/2;
		if(start[m]<=i)
			l=m+1;
		else
			r=m;
	}
	return r-1;
}

i32 CSA_Handler::character(i32 i){
	return incode[i];
}

void CSA_Handler::Extracting(i32 start,i32 len,uchar *&sequence){
	if(start+len-1>n-1){
		cerr<<"parmater error: overshot!!!"<<endl;
		sequence=NULL;
		exit(0);
	}
	sequence = new uchar[len+1];
	memset(sequence,0,(len+1)*sizeof(uchar));
	i32 k=0;
	start=inverse(start);
	for(i32 j=0;j<len;j++){
		k=phiList(start);
		sequence[j]=character(k);
		start=phi->getValue(start);
	}
}

void CSA_Handler::countSearch2(const char * pattern,i32 &L,i32 &R){
	i32 len=strlen(pattern);
	if(len==0){
		L=1;
		R=0;
		return;
	}
	uchar c=pattern[len-1];
	i32 coding=code[c];
	if(coding<0 || coding >alphabetsize-1){
		L=1;
		R=0;
		return ;
	}

	i32 Left=start[coding];
	i32 Right=start[coding+1]-1;
	i32 l0=0;
	i32 r0=0;

	for(i32 i=len-2;i>=0;i--){
		c=pattern[i];
		coding=code[c];
		if(coding<0){
			Left=1;
			Right=0;
			break;
		}
		l0=start[coding];
		if(coding==code[lastchar])
			l0=l0+1;
		r0=start[coding+1]-1;
		Right=phi->rightBoundary(Right,l0,r0);
		Left =phi->leftBoundary(Left,l0,r0);
		if(Left>Right){
			Left=1;
			Right=0;
			break;
		}
		//cout<<Left<<" "<<Right<<endl;
	}
	L=Left;
	R=Right;
	return ;
}

void CSA_Handler::countSearch(const char * pattern,i32 &L,i32 &R){
	i32 i;
	i32 jj;
	i32 middle;
	i32 left;
	i32 right;
	i32 templeft;
	i32 tempright;
	i32 mleft;
	i32 mright;
	i32 Left;
	i32 Right;
	
	i32 len = strlen(pattern);
	uchar c=pattern[len-1];
	i32 coding=code[c];
	if(coding>alphabetsize-1 || coding<0){
		L=1;
		R=0;
		return ;
	}

	Left =start[coding];
	Right=start[coding+1]-1;

	for(i=len-2;i>=0;i--){
		c=pattern[i];
		coding=code[c];
		if(coding>alphabetsize-1 || coding<0){
			L=1;
			R=0;
			return ;
		}
		left=start[coding];
		right=start[coding+1]-1;
		if(coding==code[lastchar])
			left=left+1;
		if(left>right || phi->getValue (left)>Right || phi->getValue (right)<Left){
			L=1;
			R=0;
			return ;
		}
		else{
			mleft=left;
			mright=right;
			while(mright-mleft>1){
				middle = (mleft+mright)/2;
				jj=phi->getValue(middle);
				if(jj<Left)
					mleft=middle;
				else
					mright=middle;
			}
			jj=phi->getValue (mleft);
			if(jj>=Left && jj<=Right)
				templeft=mleft;
			else
				templeft=mright;
			mleft=left;
			mright=right;
			while(mright-mleft>1){
				middle=(mleft+mright)/2;
				jj=phi->getValue (middle);
				if(jj>Right)
					mright=middle;
				else
					mleft=middle;
			}
			jj=phi->getValue (mright);
			if(jj>=Left && jj<=Right)
				tempright=mright;
			else
				tempright=mleft;
			Left=templeft;
			Right=tempright;
			if(Left>Right){
				L=1;
				R=0;
				return ;
			}
		}
	}
	if(Left>Right)
		L=1,R=0;
	else
		L=Left,R=Right;
}


i32 CSA_Handler::getAlphabetSize(){
	return alphabetsize;
}

i32 CSA_Handler::getN(){
	return n;
}

i32 CSA_Handler::sizeInByte(){
	//	return 0;
	i32 part1=0;//7*sizeof(i32)+1*sizeof(uchar)+256*sizeof(i32)*2+(alphabetsize+1)*sizeof(i32);
	i32 part2=SAL->GetMemorySize()+RankL->GetMemorySize()+phi->sizeInByte();
	return part1+part2;
}

i32 CSA_Handler::sizeInByteForCount(){
	//return 0;
	i32 part1=0;//7*sizeof(i32)+1*sizeof(uchar)+256*sizeof(i32)*2+(alphabetsize+1)*sizeof(i32);
	i32 part2=phi->sizeInByte();
	return part1+part2;
}

i32 CSA_Handler::blog(i32 x){
	i32 ans=0;
	while(x>0){
		ans++;
		x=(x>>1);
	}
	return ans;
}

i32 CSA_Handler::Save(savekit &s){
	s.writei32(n);
	s.writei32(alphabetsize);
	s.writei32(SL);
	s.writei32(L);
	s.writei32(D);
	s.writei32(RD);

	SAL->write(s);
	RankL->write(s);
	//code
	s.writei32(256);
	s.writei32array(code,256);
	//start
	s.writei32(alphabetsize+1);
	s.writei32array(start,alphabetsize+1);
	//incode
	s.writei32(alphabetsize);
	s.writei32array(incode,alphabetsize);
	//phi
	phi->write(s);
	return 0;
}

i32 CSA_Handler::Load(loadkit & s){
	s.loadi32(n);
	s.loadi32(alphabetsize);
	s.loadi32(SL);
	s.loadi32(L);
	s.loadi32(D);
	s.loadi32(RD);
	
	SAL=new InArray();
	SAL->load(s);

	RankL=new InArray();
	RankL->load(s);
	//code
	i32 len=0;
	s.loadi32(len);
	code=new i32[len];
	s.loadi32array(code,len);
	//start
	s.loadi32(len);
	start=new i32[len];
	s.loadi32array(start,len);
	//incode
	s.loadi32(len);
	incode=new i32[len];
	s.loadi32array(incode,len);

	phi=new Phi();
	phi->load(s);
	return 0;
}
	

