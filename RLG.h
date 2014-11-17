/*============================================
# Filename: RLG.h
# Ver 1.0 2014-11-17
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description:run-length plus tuning gamma coding for a gap sequence of Phi
=============================================*/
#ifndef RLG_H
#define RLG_H
#include"Coder.h"
class RLG:public Coder{
	public:
		RLG(i32 *superoffset,InArray *offset,u32 *sequence,
				InArray *samples,u8 * zerostable,i32 n,i32 a,
				i32 b,i32 &index);
		~RLG();
		void encode(i32 x);
		i32 decode(i32 & position,i32 &value);
		i32 decodeAcc(i32 position,i32 base,i32 num);
		i32 leftBoundary(i32 b,i32 l,i32 r,i32 pl);
		i32 rightBoundary(i32 b,i32 l,i32 r,i32 pr);
	private:
		i32 zeroRun(i32 &position);
		i32 getBits(i32 position,i32 bits);
		i32 blogsize(i32 x);
		void initTables();

		i32 * superoffset;
		InArray * offset;
		u32 * sequence;
		InArray * samples;
		u8 * zerostable;
		i32 n;
		i32 a;
		i32 b;
		i32 & index;
		
		u8 *decodevaluenum;
		u8 *decodebitnum;
		u8 *decoderesult;
};
#endif
