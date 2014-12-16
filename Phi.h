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
		Phi(integer * phiarray,integer n,integer blocksize);
		Phi(){}
		~Phi();
		integer getValue(const integer index);
		integer * getPhiArray();
		integer sizeInByte();
		integer leftBoundary(integer l,integer r,integer pl);
		integer rightBoundary(integer l,integer r,integer pr);
		integer load(loadkit & h);
		integer write(savekit &h);
	private:
		InArray * methods;
		integer * superoffset;
		InArray * offset;
		InArray * samples;
		u32 * sequence;
		u8 *zerostable;
		//Coder *coder[4];
		GAM * coder0;
		RLG * coder1;
		ALL1* coder2;
		RLD * coder3;

		integer n;
		integer a;
		integer b;
		integer index;
		integer *value;
		integer lenofsequence;
		integer lenofsuperoffset;
		integer maxsbs;
		
		integer blogsize(integer x);
		integer deltasize(integer x);
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

