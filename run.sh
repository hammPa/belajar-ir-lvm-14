g++ -std=c++17 main.cpp \
    `llvm-config-14 --cxxflags --ldflags --system-libs \
    --libs core orcjit native` -O3 \
    -o main && ./main


llc-14 -relocation-model=pic ./output.ll -o ./output.s

clang-14 -c ./output.s -o ./output.o
clang-14 -fPIE -pie ./output.o -o ./output_exe


rm -f main output.ll output.s output.o


echo "hasil:"
./output_exe