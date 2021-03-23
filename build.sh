CFLAGS=-g 

pushd ../build
TEMP="-lstdc++"
clang -DDEVELOPMENT $CFLAGS -o game ../src/macos_platform.cpp -lSDL2 $TEMP
popd

# gcc -o p2 -fobjc-arc -framework Cocoa -x objective-c macos_platform2.c
