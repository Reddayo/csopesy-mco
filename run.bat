if exist bin mkdir bin

g++ -g src\**.cpp -o bin\a.exe -IC:/msys64/mingw64/include -LC:/msys64/mingw64/lib -lpdcurses

bin\a.exe
