#ifndef NPHI_H
#define NPHI_H
#include"string.h"
#include"BaseClass.h"
#include"InArray.h"
#include"loadkit.h"
#include"savekit.h"
#include"Coder.h"
#include"ALL1.h"
#include"GAM.h"
#include"RLG.h"
#include"RLD.h"
class Phi{
	public:
		Phi(i32 * phiarray,i32 n,i32 blocksize);
		Phi(){}
		~Phi();
		i32 getValue(const i32 index);
		i32 * getPhiArray();
		i32 sizeInByte();
		i32 leftBoundary(i32 l,i32 r,i32 pl);
		i32 rightBoundary(i32 l,i32 r,i32 pr);
		i32 load(loadkit & h);
		i32 write(savekit &h);
	private:
		InArray * methods;
		i32 * superoffset;
		InArray * offset;
		InArray * samples;
		u32 * sequence;
		u8 *zerostable;
		//Coder *coder[4];
		GAM * coder0;
		RLG * coder1;
		ALL1* coder2;
		RLD * coder3;

		i32 n;
		i32 a;
		i32 b;
		i32 index;
		i32 *value;
		i32 lenofsequence;
		i32 lenofsuperoffset;
		i32 maxsbs;
		
		i32 blogsize(i32 x);
		i32 deltasize(i32 x);
		void initTables();
		void initCoders();

		void methodsAndSpace();
		void allocAndInit();
		void codeAndFill();

		void MethodsStatic();
		bool checkCodeAndFill_getPhiArray();
		bool checkgetValue();
};
#endif

