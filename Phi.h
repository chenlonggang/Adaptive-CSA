/*============================================
# Filename: Phi.h
# Ver 1.0 2014-10-07
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
=============================================*/
#ifndef PHI_H
#define PHI_H
#include"string.h"
#include"BaseClass.h"
#include"InArray.h"
#include"loadkit.h"
#include"savekit.h"
class Phi
{
	public:
		Phi(i32 * phiarray,i32 n,i32 blocksize);
		Phi(){}
		~Phi();
		i32 getValue(const i32 index);
		i32 * getPhiArray();
		i32 sizeInByte();
		i32 leftBoundary(i32 l,i32 L,i32 R);
		i32 rightBoundary(i32 r,i32 L,i32 R);
		i32 load(loadkit & h);
		i32 write(savekit& h);
	private:
		/*
		methods:每块的编码方法：0-->puregamma,1-->rl+gamma,2-->all1
		superoffset:超快的偏移位置。
		offset:快相对于所属超快的偏移量 
		samples:快的采样值
		sequence:编码序列。
		*/
		InArray * methods;
		i32 * superoffset;
		InArray * offset;
		InArray * samples;
		u32 * sequence;

		u16 *zerostable;
		
		u16 *decodevaluenum_gam;
		u16 *decodebitnum_gam;
		u16 *decoderesult_gam;
		u16 *decodevaluenum_rlg;
		u16 *decodebitnum_rlg;
		u16 *decoderesult_rlg;

		i32 n;
		i32 a;
		i32 b;
		i32 index;
		i32 *value;
		i32 lenofsequence;
		i32 lenofsuperoffset;
		//最大的超快的大小
		i32 maxsbs;
		i32 blogsize(i32 x);
		void initTables();

		/*计算编码方法和总空间
		  这个地方可以返回具体的快在
		  sequence中的其实位置，这样就
		  可以并行了。
		  但是建立CSA的时间不太重要，并且构建CSA的时间主要花费zai
		  计算SA上，所以是没有必要这么做的。
		*/
		void methodsAndSpace();
		/*根据第一步算出来的空间申请空间并初始化为0.
		*/
		void allocAndInit();
		/*编码Phi数组，按照计算出来的编码方法编码phi数组
		*/   
		void codeAndFill();
		void Append(i32 x);
		i32 decodeGamma(i32 &position,i32 &value);
		void Append_rlg(i32 x);
		i32 decodeGamma_rlg(i32 & position,i32 &value);

		i32 zeroRun(i32 &position);
		i32 getBits(i32 position,i32 bits);

		i32 gammaSequence(i32 position,i32 base,i32 num);
		i32 rlgSequence(i32 position,i32 base,i32 num);
		i32 all1Sequence(i32 position,i32 base,i32 num);

		i32 leftBoundary_gamma(i32 b,i32 l,i32 r,i32 pl);
		i32 leftBoundary_rlg(i32 b,i32 l,i32 r,i32 pl);
		i32 leftBoundary_all1(i32 b,i32 l,i32 r,i32 pl);

		i32 rightBoundary_gamma(i32 b,i32 l,i32 r,i32 pr);
		i32 rightBoundary_rlg(i32 b,i32 l,i32 r,i32 pr);
		i32 rightBoundary_all1(i32 b,i32 l,i32 r,i32 pr);

		/*以下函数位验证正确性只用。
		*/
		void MethodsStatic();
		bool checkCodeAndFill_getPhiArray();
		bool checkgetValue();
};
#endif
