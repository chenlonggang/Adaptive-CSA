/*============================================
# Filename: CSA.h
# Ver 1.0 2014-10-06
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
=============================================*/
#ifndef CSA_H
#define CSA_H
#include"CSA_Handler.h"
#include"BaseClass.h"
class CSA
{
	public:
		CSA(const char * filename,i32 speedlevel=1);
		CSA();
		~CSA(){};
		CSA(const CSA & right):ct(right.ct){}
		CSA & operator=(const CSA & right){ct=right.ct;return *this;}

		void Counting(const char * pattern,i32 &num);
		void Locating(const char * pattern,i32 &num,i32 *&pos);
		void Extracting(i32 start,i32 len,uchar *&sequence);

		i32 Load(const char * indexfile);
		i32 Save(const char * indexfile);

		i32 getN();
		i32 getAlphabetSize();
		i32 sizeInByte();
		i32 sizeInByteForCount();
	private:
		CSA_Handler ct;
};
#endif
