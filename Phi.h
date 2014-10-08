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
#include"BaseClass.h"
#include"loadkit.h"
#include"savekit.h"
class Phi
{
	public:
		Phi(i32 * phiarray,i32 n,i32 blocksize);
		Phi(){}
		i32 getValue(const i32 index);
		i32 load(loadkit & h);
		i32 write(savekit& h);
	private:
		i32 n;
		i32 a;
		i32 b;
		i32 *value;
};
#endif
