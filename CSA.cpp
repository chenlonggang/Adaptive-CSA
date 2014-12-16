/*============================================
# Filename: CSA.cpp
# Ver 1.0 2014-10-06
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
=============================================*/
#include"CSA.h"

CSA::CSA(const char * filename,integer speedlevel):ct(filename,128,32,speedlevel){}

CSA::CSA():ct(){}

integer CSA::getAlphabetSize(){
	return ct.getAlphabetSize();
}

integer CSA::getN(){
	return ct.getN();
}

integer CSA::sizeInByte(){
	return ct.sizeInByte();
}

integer CSA::sizeInByteForCount(){
	return ct.sizeInByteForCount();
}

double CSA::compressRatio(){
	return sizeInByte()/(getN()*1.0);
}

double CSA::compressRatioForCount(){
	return sizeInByteForCount()/(getN()*1.0);
}

integer CSA::save(const char * indexfile){
	savekit s(indexfile);
	s.writeu64(198809102510);
	ct.Save(s);
	s.close();
	return 0;
}

integer CSA::load(const char * indexfile){
	loadkit s(indexfile);
	u64 magicnum=0;
	s.loadu64(magicnum);
	if(magicnum!=198809102510){
		cerr<<"Not a CSA_index file"<<endl;
		exit(0);
	}
	ct.Load(s);
	s.close();
	return 0;
}

void CSA::counting(const char * pattern,integer &num){
	ct.Counting(pattern,num);
}

integer * CSA::locating(const char * pattern,integer  &num){
	return ct.Locating(pattern,num);
}

uchar * CSA::extracting(integer start,integer len){
	return ct.Extracting(start,len);
}

