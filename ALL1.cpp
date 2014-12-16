/*============================================
# Filename: ALL1.cpp
# Ver 1.0 2014-11-17
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description:coding methods for a gap sequence of Phi.
All the gap is 1.
=============================================*/
#include"ALL1.h"
ALL1::ALL1(integer *superoffset,InArray *offset,u32 *sequence,
		InArray *samples,u8* zerostable,integer n,integer a,
		integer b,integer &index):index(index){
	this->superoffset=superoffset;
	this->offset=offset;
	this->sequence=sequence;
	this->samples=samples;
	this->zerostable=zerostable;
	this->n=n;
	this->a=a;
	this->b=b;
}

integer ALL1::decodeAcc(integer position,integer base,integer num){
	return (base+num)%n;
}

integer ALL1::leftBoundary(integer b,integer l,integer r,integer pl){
	integer m=0;
	integer ans=0;
	integer L=this->b;
	if(r>b*L-1)
		r=b*L-1;
	ans=r+1;
	integer x=samples->GetValue(b-1);
	m=(b-1)*L;

	if(l>m){
		integer step=l-m;
		x=(x+l-m+n)%n;
		m=l;
		if(pl<=(x+L-1-step+n)%n)
			ans=m+(pl-x+n)%n;
	}
	else{
		if(pl<=(x+L-1)%n)														
			ans=m+pl-x;
	}
	return ans;
}

integer ALL1::rightBoundary(integer b,integer l,integer r,integer pr){
	integer L=this->b;
	integer m=0;
	integer ans=l-1;
	if(r>(b+1)*L-1)
		r=(b+1)*L-1;
	integer x=samples->GetValue(b);
	m=b*L;
	
	if(pr<=(x+r-m+n)%n)
		ans=m+(pr-x)%n;
	else
		ans=r;
	return ans;
}
