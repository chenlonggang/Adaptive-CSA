/*============================================
# Filename: GAM.h
# Ver 1.0 2014-11-17
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: gamma coding for a sequence of gap of Phi.
=============================================*/
#ifndef GAM_H
#define GAM_H
#include"Coder.h"
class GAM:public Coder{
	public:
		GAM(integer *superoffset,InArray *offset,u32 *sequence,
				InArray *samples,u8 * zerostable,integer n,integer a,
				integer b,integer &index);
		~GAM();
		void encode(integer x);
		integer decode(integer & position,integer &value);
		integer decodeAcc(integer position,integer base,integer num);
		integer leftBoundary(integer b,integer l,integer r,integer pl);
		integer rightBoundary(integer b,integer l,integer r,integer pr);
	private:
		integer zeroRun(integer &position);
		integer getBits(integer position,integer bits);
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


