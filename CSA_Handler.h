/*============================================
# Filename: CSA_Handler.h
# Ver 1.0 2014-10-06
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
=============================================*/
#ifndef CSA_Handler_H
#define CSA_Handler_H
#include"BaseClass.h"
#include"InArray.h"
#include"Phi.h"
#include"UseCount.h"
class CSA_Handler
{
	public:
		CSA_Handler(){};
		CSA_Handler(const char*sourcefile,integer L=128,integer D=32,integer speedlevel=1);
		CSA_Handler(const CSA_Handler &);
		CSA_Handler & operator =(const CSA_Handler &);
		~CSA_Handler();

		void Counting(const char * pattern,integer &num);
		integer * Locating(const char * pattern,integer &num);
		uchar * Extracting(integer start,integer len);

		integer Save(savekit & s);
		integer Load(loadkit & s);

		integer getAlphabetSize();
		integer getN();
		integer sizeInByte();
		integer sizeInByteForCount();
	private:
		//由位置得到该位置开始的后缀的排名
		integer inverse(integer position);
		//得到排名位sa的后缀所属的list
		integer phiList(integer sa);
		//得到搜排名位sa的后缀的首字母
		integer character(integer sa);
		//返回SA[sa]
		integer lookUp(integer sa);
		//Counting的实现，采用直接的二分办法
		void countSearch(const char * pattern,integer &L,integer &R);
		//Counting的实现，先在采样点上二分，然后在具体的block内二分
		void countSearch2(const char * pattern,integer &L,integer &R);
		//读取文件，初始化各种表。
		uchar * getFile(const char * filename);
		void statics(uchar * T);
		//计算出Phi数组
		integer * phiArray(integer * SA,uchar * T);
		//根据gap序列中1的比例，决定threshhold。
		void computerPar(integer * phi);
		//采样SA and Rank
		void sampleSAAndRank(integer *SA);
		integer blog(integer x);
		void deletePointers();
		void assignDataMembers(const CSA_Handler & h);
		//分别表示:Phi超快的大小，块大小，SA采样步长，Rank采样步长
		integer SL,L,D,RD;
		//分别表示SA的采样数组和Rank的采样数组
		InArray * SAL;
		InArray * RankL;
		//文件大小
		integer n;
		//映射表。
		integer *code;
		integer *incode;
		//C表
		integer *start;
		//字母表大小
		integer alphabetsize;
		//最后一个字符
		uchar lastchar;
		//Phi结构
		Phi * phi;
		integer speedlevel;
		//operator=和拷贝构造实现浅拷贝，引用计数在这里实现
		UseCount u;
};
#endif

