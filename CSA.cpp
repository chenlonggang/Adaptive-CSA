/*============================================
# Filename: CSA.cpp
# Ver 1.0 2014-10-06
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
=============================================*/
#include"CSA.h"

CSA::CSA(const char * filename,i32 speedlevel):ct(filename,128,32,speedlevel){}

CSA::CSA():ct(){}

i32 CSA::getN()
{
	return ct.getN();
}

i32 CSA::sizeInByte()
{
	return ct.sizeInByte();
}

i32 CSA::sizeInByteForCount()
{
	return ct.sizeInByteForCount();
}

i32 CSA::Save(const char * indexfile)
{
	savekit s(indexfile);
	s.writeu64(198809102510);
	ct.Save(s);
	s.close();
	return 0;
}

i32 FM::Load(const char * indexfile)
{
	loadkit s(indexfile);
	u64 magicnum=0;
	s.loadu64(magicnum);
	if(magicnum==198809102510)
	{
		cerr<<"Not a CSA_index file"<<endl;
		exit(0);
	}
	wt.Load(s);
	s.close();
	return 0;
}

void CSA::Counting(const char * pattern,i32 &num)
{
	ct.Counting(pattern,num);
}

void CSA::Locating(const char * pattern,i32  &num,int *&pos)
{
	ct.Locating(pattern,num,pos);
}

void CSA::Extracting(i32 start,i32 len,i32 *&sequence)
{
	ct.Extracting(start,len,sequence);
}