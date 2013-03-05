
# Do the following to bootstrap:
#
#    1) Unpack bootstrap.tar using "tar xvf bootstrap.tar"
#    2) Run bootstrap.sh "sh bootstrap.sh"

gcc -g -c -I. Compiler.ez.c
gcc -g -c -I. Declaration.ez.c
gcc -g -c -I. Easy_C.ez.c
gcc -g -c -I. Expression.ez.c
gcc -g -c -I. EZCC.ez.c
gcc -g -c -I. Parse.ez.c
gcc -g -c -I. Statement.ez.c
gcc -g -c -I. Token.ez.c
gcc -g -c -I. Unix.ez.c
gcc -g -c -o Easy_C_C.ez.o -I. Easy_C_C.c
gcc -g -c -o Unix_C.ez.o -I. Unix_C.c
echo Linking Bootstrap Compiler
gcc -g -o ezcc ezcc.ez.c EZCC.ez.o Compiler.ez.o Easy_C.ez.o Declaration.ez.o Token.ez.o Parse.ez.o Expression.ez.o Statement.ez.o Unix.ez.o Easy_C_C.ez.o Unix_C.ez.o
echo Using bootstrap to compile up new compiler.
./ezcc -S .ez -o ezcc.ez EZCC
