CFLAGS="-g -fdiagnostics-absolute-paths"
CFLAGS="${CFLAGS} -DPROFILING=1"

IGNORES="-Wno-c++11-compat-deprecated-writable-strings"

pushd ../build

TEMP="-lstdc++"
clang -DDEVELOPMENT $CFLAGS -o game ../src/macos_platform.cpp -lSDL2 $TEMP $IGNORES


clang -DDEVELOPMENT $CFLAGS -o cooker ../src/cooker.cpp



popd

# gcc -o p2 -fobjc-arc -framework Cocoa -x objective-c macos_platform2.c
