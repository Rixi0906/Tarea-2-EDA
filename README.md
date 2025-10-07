INSTRUCCIONES

1- cd ~/Tarea2/Tarea-2-EDA/cpp 
2- cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
3- cmake --build build -j
4- cd build/

Ahora para cada dataset se usa un comando distinto desde cpp/build/.
500K: ./test ../../codigos/codes_500K.txt 500000
1M: ./test ../../codigos/codes_1M.txt 1000000
10M: ./test ../../codigos/codes_10M.txt 10000000
