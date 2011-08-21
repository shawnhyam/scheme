DEPS="scheme_impl.o read.o write.o meta_circ.o"
redo-ifchange $DEPS
clang -Wall -o $3 $DEPS
