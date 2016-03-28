# This file is auto-generated, changes made to it will be lost. Please edit makebuildscripts.py instead.

if [ -z "${SNIPER_ROOT}" ] ; then SNIPER_ROOT=$(readlink -f "$(dirname "${BASH_SOURCE[0]}")/..") ; fi

GRAPHITE_CC="cc"
GRAPHITE_CFLAGS="-mfpmath=sse -msse -msse2 -mno-sse4 -mno-sse4.1 -mno-sse4.2 -mno-sse4a -mno-avx -I${SNIPER_ROOT}/include -m32"
GRAPHITE_CXX="g++"
GRAPHITE_CXXFLAGS="-mfpmath=sse -msse -msse2 -mno-sse4 -mno-sse4.1 -mno-sse4.2 -mno-sse4a -mno-avx -I${SNIPER_ROOT}/include -m32"
GRAPHITE_LD="g++"
GRAPHITE_LDFLAGS="-static -L${SNIPER_ROOT}/lib -pthread -m32"
GRAPHITE_LD_LIBRARY_PATH=""
GRAPHITE_UPCCFLAGS="-I${SNIPER_ROOT}/include -m32 -link-with='g++ -static -L${SNIPER_ROOT}/lib -pthread -m32'"
PIN_HOME="/home/prak/Desktop/sniper_flexiway/pin_kit"
SNIPER_CC="cc"
SNIPER_CFLAGS="-mfpmath=sse -msse -msse2 -mno-sse4 -mno-sse4.1 -mno-sse4.2 -mno-sse4a -mno-avx -I${SNIPER_ROOT}/include -m32"
SNIPER_CXX="g++"
SNIPER_CXXFLAGS="-mfpmath=sse -msse -msse2 -mno-sse4 -mno-sse4.1 -mno-sse4.2 -mno-sse4a -mno-avx -I${SNIPER_ROOT}/include -m32"
SNIPER_LD="g++"
SNIPER_LDFLAGS="-static -L${SNIPER_ROOT}/lib -pthread -m32"
SNIPER_LD_LIBRARY_PATH=""
SNIPER_UPCCFLAGS="-I${SNIPER_ROOT}/include -m32 -link-with='g++ -static -L${SNIPER_ROOT}/lib -pthread -m32'"
