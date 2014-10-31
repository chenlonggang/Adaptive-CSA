#ifndef CODER_H
#define CODER_H
#include"BaseClass.h"
#include"InArray.h"
class Coder{
	public:
		virtual ~Coder(){};
		virtual void encode(i32 x)=0;
		virtual i32 decode(i32 &position,i32 &value)=0;
		virtual i32 decodeAcc(i32 position,i32 base,i32 num)=0;
		virtual i32 leftBoundary(i32 b,i32 l,i32 r,i32 pl)=0;
		virtual i32 rightBoundary(i32 b,i32 l,i32 r,i32 pr)=0;
};
#endif
