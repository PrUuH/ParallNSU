CFLAG ?= -fopenmp
MATRIX_SIZE ?= 20000
NTHREADS ?= 80

# task3_each_section: test.cpp FORCE
# 	g++ -DMATRIX_SIZE=$(MATRIX_SIZE) -DNTHREADS=$(NTHREADS) $(CFLAG) -o $@ $<

task3_one_section: test1.cpp FORCE
	g++ -DMATRIX_SIZE=$(MATRIX_SIZE) -DNTHREADS=$(NTHREADS) $(CFLAG) -o $@ $<	

FORCE: