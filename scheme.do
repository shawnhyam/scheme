DEPS="scheme.o"
redo-ifchange $DEPS
gcc -o $3 $DEPS
