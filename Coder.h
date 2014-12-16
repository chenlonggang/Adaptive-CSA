#ifndef CODER_H
#define CODER_H
#include"BaseClass.h"
#include"InArray.h"
class Coder{
	public:
		virtual ~Coder(){};
		virtual void encode(integer x)=0;
		virtual integer decode(integer &position,integer &value)=0;
		virtual integer decodeAcc(integer position,integer base,integer num)=0;
		virtual integer leftBoundary(integer b,integer l,integer r,integer pl)=0;
		virtual integer rightBoundary(integer b,integer l,integer r,integer pr)=0;
};
#endif
