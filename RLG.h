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
		RLG(integer *superoffset,InArray *offset,u32 *sequence,
				InArray *samples,u8 * zerostable,integer n,integer a,
				integer b,integer &index);
		~RLG();
		void encode(integer x);
		integer decode(integer & position,integer &value);
		integer decodeAcc(integer position,integer base,integer num);
		integer leftBoundary(integer b,integer l,integer r,integer pl);
		integer rightBoundary(integer b,integer l,integer r,integer pr);
	private:
		void appendBinary(u64 data,integer valuewidth);
		integer zeroRun(integer &position);
		u64 getBits(integer position,integer bits);
		integer blogsize(integer x);
		void initTables();

		integer * superoffset;
		InArray * offset;
		u32 * sequence;
		InArray * samples;
		u8 * zerostable;
		integer n;
		integer a;
		integer b;
		integer & index;
		
		u8 *decodevaluenum;
		u8 *decodebitnum;
		u8 *decoderesult;
};
#endif
