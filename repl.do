DEPS="scheme_impl.o repl.o"
redo-ifchange $DEPS
clang -Wall -o $3 $DEPS
