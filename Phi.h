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
class Phi
{
	public:
		Phi(i32 * phiarray,i32 n,i32 blocksize);
		i32 getValue(const i32 index);
		i32 & operator [](const i32 index);
	private:
}
