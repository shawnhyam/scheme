DEPS="compile.o eval.o main.o obj.o read.o write.o"
redo-ifchange $DEPS
gcc -o $3 $DEPS