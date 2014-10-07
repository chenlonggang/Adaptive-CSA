CC=g++
CFLAGS=  -O3 -Wall
my_csa:ds_ssortr main.o csa.a 
	g++ main.o csa.a -o my_csa 

csa.a:savekit.o loadkit.o InArray.o Phi.o UseCount.o CSA_Handler.o CSA.o  ds_ssort/ds.o ds_ssort/globals.o ds_ssort/helped.o ds_ssort/shallow.o ds_ssort/ds.o ds_ssort/globals.o ds_ssort/helped.o ds_ssort/shallow.o ds_ssort/deep2.o ds_ssort/blind2.o
	ar rc csa.a CSA.o CSA_Handler.o UseCount.o savekit.o loadkit.o InArray.o Phi.o   ds_ssort/ds.o ds_ssort/globals.o ds_ssort/helped.o ds_ssort/shallow.o ds_ssort/ds.o ds_ssort/globals.o ds_ssort/helped.o ds_ssort/shallow.o ds_ssort/deep2.o ds_ssort/blind2.o

%.o:%.cpp *.h
	$(CC) -c $(CFLAGS) $< -o $@
main.o:main.cpp  CSA.h
	g++ -c main.cpp 
clean:
	rm *.a  *.o my_csa ds_ssort/*.a ds_ssort/*.o ds_ssort/bwt ds_ssort/ds ds_ssort/testlcp ds_ssort/unbwt
ds_ssortr:
	make -C ./ds_ssort/; cp ./ds_ssort/ds_ssort.a .

