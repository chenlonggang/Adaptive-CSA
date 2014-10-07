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

CSA_Handler::CSA_Handler(const char * sourcefile,i32 L,i32 D,i32 speedlevel)
{
	this->SL=L*18;
	this->L=L;
	this->D=D;
	this->RD=D*16;

	this->code = new i32[256];
	memset(code,0,256*sizeof(i32));

	thia->slphabetsize=0;
	uchar * T=NULL;
	//读取文件
	T=getFile(sourcefile);
	//统计，初始化code/incode/start表
	statics(T);
	i32 * SA = new i32[n];
	//计算后缀数组 
	ds_ssort(T,SA,n);
	//计算Phi数组
	i32 * phiarray = phiArray();
	
	delete [] T;//T is useless now
	T=NULL;
	//计算gap中1的比例，依次决定参数 
	//ratio暂时不决定D和RD，只决定L和SL.
	computerPar(phiarray);
	//采样SAL和RankL
	sampleSAAndRank();
	//from now on ,SA is useless;
	delete [] SA;
	SA=NULL;
	//创建Phi结构。参数：
	//phiArray:Phi数组
	//n:Phi数组长度
	//L:块大小，超快大小固定位块大小的18倍,不用传.
	phi = new Phi(phiArray,n,L);
	
	delete [] phiarray;
	phiarray=NULL;

}

void CSA_Handler::computePar(i32 * phi)
{
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
	if(speedlevel==0)
	{
		if(ratio<0.2)
			multi=1;
		else if(ratio<0.4)
			multi=2;
		else
			multi=4;
	}
	if(speedlevel==1)
	{
		if(ratio<0.3)
			multi=1;
		else if(ratio<0.5)
			multi=2;
		else
			multi=4;
	}
	if(speedlevel==2)
	{
		if(ratio<0.5)
			multi=1;
		else if(ratio<0.7)
			multi=2;
		else
			multi=4;
	}
	this->L=this->L*multi;
	this->SL=this->L*18;
	this->D=this->D*multi;
	this->RD=this->D*16;
}

void CSA_Handle::sampleSAAndRank()
{
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


i32 * CSA_Handler::phiArray()
{
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
	for(i32 i=0;i<n;i++)
	{
		pos=SA[i];
		if(pos==0)
		{
			h=i;
			continue;
		}
		c=T[pos-1];
		phi[temp[code[c]]++]=i;
	}
	phi[index]=h;
	delete [] temp;
	temp=NULL;
}



uchar * CSA_Handler::getFile(const char * filename)
{
	FILE * fp=fopen(filename,"r+");
	if(fp==NULL)
	{
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
	if(num!=n)
	{
		cerr<<"broken while reading file"<<endl;
		exit(0);
	}

	fclose(fp);
	return T;
}

void CSA_Handler::statics(uchar *T)
{
	for(i32 i=0;i<n;i++)
		code[T[i]]++;

	for(i32 i=0;i<256;i++)
		if(code[i]!=0)
			alphabetsize++;
	this->start=new i32[alphabetsize+1];
	this->start[alphabetsize]=n;
	this->start[0]=0;
	i32 k=1;
	i32 pre=1;

	for(i32 i=0;i<256;i++)
		if(code[i]!=0)
		{
			start[k]=pre+code[i];
			pre=start[k];
			k++;
		}

	this->incode = new i32[alphabetsize];
	k=0;
	for(i32 i=0;i<256;i++)
		if(code[i]!=0)
		{
			code[i]=k;
			incode[k]=i;
			k++;
		}
		else
			code[i]=-1;
	lastchar=T[n-1];
}

void CSA_Handler::Counting(const char * pattern,i32 &num)
{
	i32 L=0;
	i32 R=0;
	countSearch(pattern,L,R);
	num=R-L+1;
}

i32 CSA_Handler::lookUp(i32 sa)
{
	i32 step=0;
	while(i%D!=0)
	{
		i=phi->GetValue(i);
		step++;
	}
	i=i/D;
	return (n+SAL->GetValue(i)-step)%n;
}

void CSA_Handler::Locating(const char * pattern,i32 &num,i32 *&pos)
{
	i32 L=0;
	i32 R=0;
	this->countSearch(pattern,L,R);
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
		sa=Phi->GetValue(sa);
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

void CSA_Handler::Extracting(i32 start,i32 len,uchar *&sequence)
{
	if(start+len-1>n-1){
		cerr<<"parmater error: overshot!!!"<<endl;
		sequence=NULL;
		exit(0);
	}
	sequence = new uchar[len];
	memset(sequence,0,len*sizeof(uchar));
	i32 k=0;
	for(i32 j=0;j<len;j++){
		k=phiList(start);
		sequence[j]=character(k);
		start=phi->GetValue(start);
	}
}

