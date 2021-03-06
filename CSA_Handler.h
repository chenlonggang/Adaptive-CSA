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
		CSA_Handler(const char*sourcefile,i32 L=128,i32 D=32,i32 speedlevel=1);
		CSA_Handler(const CSA_Handler &);
		CSA_Handler & operator =(const CSA_Handler &);
		~CSA_Handler();

		void Counting(const char * pattern,i32 &num);
		i32 * Locating(const char * pattern,i32 &num);
		uchar * Extracting(i32 start,i32 len);

		i32 Save(savekit & s);
		i32 Load(loadkit & s);

		i32 getAlphabetSize();
		i32 getN();
		i32 sizeInByte();
		i32 sizeInByteForCount();
	private:
		//由位置得到该位置开始的后缀的排名
		i32 inverse(i32 position);
		//得到排名位sa的后缀所属的list
		i32 phiList(i32 sa);
		//得到搜排名位sa的后缀的首字母
		i32 character(i32 sa);
		//返回SA[sa]
		i32 lookUp(i32 sa);
		//Counting的实现，采用直接的二分办法
		void countSearch(const char * pattern,i32 &L,i32 &R);
		//Counting的实现，先在采样点上二分，然后在具体的block内二分
		void countSearch2(const char * pattern,i32 &L,i32 &R);
		//读取文件，初始化各种表。
		uchar * getFile(const char * filename);
		void statics(uchar * T);
		//计算出Phi数组
		i32 * phiArray(i32 * SA,uchar * T);
		//根据gap序列中1的比例，决定threshhold。
		void computerPar(i32 * phi);
		//采样SA and Rank
		void sampleSAAndRank(i32 *SA);
		i32 blog(i32 x);
		void deletePointers();
		void assignDataMembers(const CSA_Handler & h);
		//分别表示:Phi超快的大小，块大小，SA采样步长，Rank采样步长
		i32 SL,L,D,RD;
		//分别表示SA的采样数组和Rank的采样数组
		InArray * SAL;
		InArray * RankL;
		//文件大小
		i32 n;
		//映射表。
		i32 *code;
		i32 *incode;
		//C表
		i32 *start;
		//字母表大小
		i32 alphabetsize;
		//最后一个字符
		uchar lastchar;
		//Phi结构
		Phi * phi;
		i32 speedlevel;
		//operator=和拷贝构造实现浅拷贝，引用计数在这里实现
		UseCount u;
};
#endif

