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
		CSA(const char * filename,integer speedlevel=1);
		CSA();
		~CSA(){};
		CSA(const CSA & right):ct(right.ct){}
		CSA & operator=(const CSA & right){ct=right.ct;return *this;}

		void counting(const char * pattern,integer &num);
		integer *locating(const char * pattern,integer &num);
		uchar * extracting(integer start,integer len);

		integer load(const char * indexfile);
		integer save(const char * indexfile);

		integer getN();
		integer getAlphabetSize();
		integer sizeInByte();
		integer sizeInByteForCount();
		double compressRatio();
		double compressRatioForCount();
	private:
		CSA_Handler ct;
};
#endif
