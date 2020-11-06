CFLAGS=-g

pushd ../build
clang $CFLAGS -o game ../src/macos_platform.c -lSDL2
popd

# gcc -o p2 -fobjc-arc -framework Cocoa -x objective-c macos_platform2.c
