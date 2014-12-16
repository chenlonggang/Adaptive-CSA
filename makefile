CC=g++
CFLAGS=  -O3 -Wall
ada_csa:div main.o csa.a 
	g++ main.o csa.a -o ada_csa 

csa.a:savekit.o loadkit.o InArray.o Phi.o Coder.o ALL1.o GAM.o RLG.o RLD.o UseCount.o CSA_Handler.o CSA.o divsufsort.o sssort.o trsort.o utils.o
	ar rc csa.a CSA.o CSA_Handler.o UseCount.o savekit.o loadkit.o InArray.o Phi.o Coder.o ALL1.o GAM.o RLG.o RLD.o divsufsort.o sssort.o trsort.o utils.o
%.o:%.cpp *.h
	$(CC) -c $(CFLAGS) $< -o $@
main.o:main.cpp  CSA.h
	g++ -c main.cpp 
clean:
	rm *.a  *.o ada_csa ./divsufsort/div32/*.a ./divsufsort/div32/*.o
div:
	make -C ./divsufsort/div32/;cp divsufsort/div32/libdivsufsort.a .;ar x libdivsufsort.a;rm libdivsufsort.a 

