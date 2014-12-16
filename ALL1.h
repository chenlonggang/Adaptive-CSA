/*============================================
# Filename: ALL1.h
# Ver 1.0 2014-11-17
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: coding methods for a gap sequence of Phi.
All the gap is 1.
=============================================*/
#ifndef ALL1_H
#define ALL1_H
#include"Coder.h"
class ALL1:public Coder{
	public :
		ALL1(integer *superoffset,InArray *offset,u32 *sequence,
				InArray *samples,u8 * zerostable,integer n,integer a,
				integer b,integer &index);
		~ALL1(){};
		void encode(integer x){};
		integer decode(integer & position,integer &value){return 0;};
		integer decodeAcc(integer position,integer base,integer num);
		integer leftBoundary(integer b,integer l,integer r,integer pl);
		integer rightBoundary(integer b,integer l,integer r,integer pr);
	private:
		integer zeroRun(integer &position);
		integer getBits(integer position,integer bits);
		
		integer * superoffset;
		InArray * offset;
		u32 * sequence;
		InArray * samples;
		u8 * zerostable;
		integer n;
		integer a;
		integer b;
		integer & index;
};
#endif


