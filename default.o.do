redo-ifchange $1.c
gcc -IAODBM -std=c99 -falign-functions=16 -MD -MF $1.d -c -o $3 $1.c
read DEPS <$1.d
redo-ifchange ${DEPS#*:}
