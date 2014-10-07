/*============================================
# Filename: Phi.cpp
# Ver 1.0 2014-10-07
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
=============================================*/
#include"Phi.h"
Phi::Phi(i32 * phiarray,i32 n,i32 bs){
	this->n=n;
	this->b=bs;
	this->a=18*b;
	value=new i32[n];
	for(i32 i=0;i<n;i++)
		value[i]=phiarray[i];
}
i32 Phi::getValue(const i32 index){
	return value[index];
}

