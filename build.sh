CFLAGS=-g

pushd ../build
clang -DDEVELOPMENT $CFLAGS -o game ../src/macos_platform.cpp -lSDL2
popd

# gcc -o p2 -fobjc-arc -framework Cocoa -x objective-c macos_platform2.c
