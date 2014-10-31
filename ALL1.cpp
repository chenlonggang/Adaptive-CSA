#include"ALL1.h"
ALL1::ALL1(i32 *superoffset,InArray *offset,u32 *sequence,
		InArray *samples,u8* zerostable,i32 n,i32 a,
		i32 b,i32 &index):index(index){
	this->superoffset=superoffset;
	this->offset=offset;
	this->sequence=sequence;
	this->samples=samples;
	this->zerostable=zerostable;
	this->n=n;
	this->a=a;
	this->b=b;
	//this->index=index;
}

i32 ALL1::decodeAcc(i32 position,i32 base,i32 num){
	return (base+num)%n;
}

i32 ALL1::leftBoundary(i32 b,i32 l,i32 r,i32 pl){
	i32 m=0;
	i32 ans=0;
	i32 L=this->b;
	if(r>b*L-1)
		r=b*L-1;
	ans=r+1;
	i32 x=samples->GetValue(b-1);
	m=(b-1)*L;

	if(l>m){
		i32 step=l-m;
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

i32 ALL1::rightBoundary(i32 b,i32 l,i32 r,i32 pr){
	i32 L=this->b;
	i32 m=0;
	i32 ans=l-1;
	if(r>(b+1)*L-1)
		r=(b+1)*L-1;
	i32 x=samples->GetValue(b);
	m=b*L;
	
	if(pr<=(x+r-m+n)%n)
		ans=m+(pr-x)%n;
	else
		ans=r;
	return ans;
}
