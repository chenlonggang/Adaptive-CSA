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
#include"divsufsort64.h"
#include"CSA_Handler.h"
#include<string.h>
CSA_Handler::CSA_Handler(const char * sourcefile,integer L,integer D,integer speedlevel):u(){
	if(speedlevel<0)
		speedlevel=0;
	if(speedlevel>2)
		speedlevel=2;
	this->speedlevel=speedlevel;
	this->SL=L*18;
	this->L=L;
	this->D=D;
	this->RD=D*16;

	this->code = new integer[256];
	memset(code,0,256*sizeof(integer));

	this->alphabetsize=0;
	uchar * T=NULL;
	T=getFile(sourcefile);
	statics(T);
	
	integer * SA = new integer[n];
	divsufsort64(T,SA,n);
	integer * phiarray = phiArray(SA,T);
	delete [] T;
	T=NULL;

	computerPar(phiarray);
	sampleSAAndRank(SA);
	delete [] SA;
	SA=NULL;

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

void CSA_Handler::computerPar(integer * phi){
	integer pre=0;
	integer gap=0;
	integer num=0;
	for(integer i=0;i<n;i++)
	{
		gap=phi[i]-pre;
		if(gap==1)
			num++;
		pre=phi[i];
	}
	double ratio = (num*1.0)/n;
	integer multi=1;
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
	this->L=this->L*multi;
	this->SL=this->L*18;
  	//cout<<"	CSA_Handler--136: multi:  "<<multi<<endl;
}

void CSA_Handler::sampleSAAndRank(integer * SA){
	integer i=0;
	integer j=0;
	integer step1=D;
	integer step2=RD;
	SAL = new InArray(n/step1+1,blog(n));
	RankL=new InArray(n/step2+1,blog(n));
	for(i=0,j=0;i<n;i=i+step1,j++)
		SAL->SetValue(j,SA[i]);
	for(i=0;i<n;i++)
		if(SA[i]%step2==0)
			RankL->SetValue(SA[i]/step2,i);
}


integer * CSA_Handler::phiArray(integer *SA,uchar * T){
	integer * phi = new integer[n];
	memset(phi,0,n*sizeof(integer));
	integer * temp = new integer[alphabetsize+1];
	for(integer i=0;i<alphabetsize+1;i++)
		temp[i]=this->start[i];
	integer index = temp[code[lastchar]];
	temp[code[lastchar]]++;
	integer h=0;
	uchar c=0;
	integer pos=0;
	for(integer i=0;i<n;i++){
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
	fseek(fp,0,SEEK_END);
	this->n=ftell(fp);
	fseek(fp,0,SEEK_SET);

	//integer overshot =init_ds_ssort(500,2000);
	//uchar * T=new uchar[n+overshot];
	uchar * T=new uchar[n];
	integer e=0;
	integer num=0;
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
	for(integer i=0;i<n;i++)
		code[T[i]]++;

	for(integer i=0;i<256;i++)
		if(code[i]!=0)
			alphabetsize++;
	this->start=new integer[alphabetsize+1];
	this->start[alphabetsize]=n;
	this->start[0]=0;
	integer k=1;
	integer pre=0;

	for(integer i=0;i<256;i++)
		if(code[i]!=0){
			start[k]=pre+code[i];
			pre=start[k];
			k++;
		}

	this->incode = new integer[alphabetsize];
	k=0;
	for(integer i=0;i<256;i++)
		if(code[i]!=0){
			code[i]=k;
			incode[k]=i;
			k++;
		}
		else
			code[i]=-1;
	lastchar=T[n-1];
}

void CSA_Handler::Counting(const char * pattern,integer &num){
	integer L=0;
	integer R=0;
	countSearch2(pattern,L,R);
	num=R-L+1;
}

integer CSA_Handler::lookUp(integer i){
	integer step=0;
	while(i%D!=0){
		i=phi->getValue(i);
		step++;
	}
	i=i/D;
	return (n+SAL->GetValue(i)-step)%n;
}

integer * CSA_Handler::Locating(const char * pattern,integer &num){
	integer L=0;
	integer R=0;
	this->countSearch2(pattern,L,R);
	num=R-L+1;
	if(L>R)
		return NULL;
	integer *pos=new integer[num];
	for(integer i=L;i<=R;i++)
		pos[i-L]=lookUp(i);
	return pos;
}

integer CSA_Handler::inverse(integer pos){
	integer anchor = pos/RD;
	integer p=anchor*RD;
	integer sa=RankL->GetValue(anchor);
	while(p<pos){
		sa=phi->getValue(sa);
		p++;
	}
	return sa;
}

integer CSA_Handler::phiList(integer i){
	integer l=0;
	integer r=alphabetsize;
	integer m=0;
	while(l<r){
		m=(l+r)/2;
		if(start[m]<=i)
			l=m+1;
		else
			r=m;
	}
	return r-1;
}

integer CSA_Handler::character(integer i){
	return incode[i];
}

uchar * CSA_Handler::Extracting(integer start,integer len){
	if(start+len-1>n-1){
		cerr<<"parmater error: overshot!!!"<<endl;
		return NULL;
	}
	uchar *sequence = new uchar[len+1];
	memset(sequence,0,(len+1)*sizeof(uchar));
	integer k=0;
	start=inverse(start);
	for(integer j=0;j<len;j++){
		k=phiList(start);
		sequence[j]=character(k);
		start=phi->getValue(start);
	}
	return sequence;
}

void CSA_Handler::countSearch2(const char * pattern,integer &L,integer &R){
	integer len=strlen(pattern);
	if(len==0){
		L=1;
		R=0;
		return;
	}
	uchar c=pattern[len-1];
	integer coding=code[c];
	if(coding<0 || coding >alphabetsize-1){
		L=1;
		R=0;
		return ;
	}

	integer Left=start[coding];
	integer Right=start[coding+1]-1;
	integer l0=0;
	integer r0=0;

	for(integer i=len-2;i>=0;i--){
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
	}
	L=Left;
	R=Right;
	return ;
}

void CSA_Handler::countSearch(const char * pattern,integer &L,integer &R){
	integer i;
	integer jj;
	integer middle;
	integer left;
	integer right;
	integer templeft;
	integer tempright;
	integer mleft;
	integer mright;
	integer Left;
	integer Right;
	
	integer len = strlen(pattern);
	uchar c=pattern[len-1];
	integer coding=code[c];
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


integer CSA_Handler::getAlphabetSize(){
	return alphabetsize;
}

integer CSA_Handler::getN(){
	return n;
}

integer CSA_Handler::sizeInByte(){
	integer part1=7*sizeof(integer)+1*sizeof(uchar)+256*sizeof(integer)*2+(alphabetsize+1)*sizeof(integer);
	integer part2=SAL->GetMemorySize()+RankL->GetMemorySize()+phi->sizeInByte();
	return part1+part2;
}

integer CSA_Handler::sizeInByteForCount(){
	integer part1=7*sizeof(integer)+1*sizeof(uchar)+256*sizeof(integer)*2+(alphabetsize+1)*sizeof(integer);
	integer part2=phi->sizeInByte();
	return part1+part2;
}

integer CSA_Handler::blog(integer x){
	integer ans=0;
	while(x>0){
		ans++;
		x=(x>>1);
	}
	return ans;
}

integer CSA_Handler::Save(savekit &s){
	s.writeinteger(n);
	s.writeinteger(alphabetsize);
	s.writeinteger(SL);
	s.writeinteger(L);
	s.writeinteger(D);
	s.writeinteger(RD);

	SAL->write(s);
	RankL->write(s);

	s.writeinteger(256);
	s.writeintegerarray(code,256);

	s.writeinteger(alphabetsize+1);
	s.writeintegerarray(start,alphabetsize+1);

	s.writeinteger(alphabetsize);
	s.writeintegerarray(incode,alphabetsize);

	phi->write(s);
	return 0;
}

integer CSA_Handler::Load(loadkit & s){
	s.loadinteger(n);
	s.loadinteger(alphabetsize);
	s.loadinteger(SL);
	s.loadinteger(L);
	s.loadinteger(D);
	s.loadinteger(RD);
	
	SAL=new InArray();
	SAL->load(s);

	RankL=new InArray();
	RankL->load(s);
	
	integer len=0;
	s.loadinteger(len);
	code=new integer[len];
	s.loadintegerarray(code,len);

	s.loadinteger(len);
	start=new integer[len];
	s.loadintegerarray(start,len);

	s.loadinteger(len);
	incode=new integer[len];
	s.loadintegerarray(incode,len);
	
	phi=new Phi();
	phi->load(s);
	return 0;
}
	

