DEPS="scheme_impl.o read.o write.o repl.o"
redo-ifchange $DEPS
clang -Wall -o $3 $DEPS
