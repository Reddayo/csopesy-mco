# CSOPESY: Marquee console

**CESAR**, Jusper Angelo Manuel 

**LLOVIT**, Benn Erico Bernardo

**MARQUESES**, Lorenz Bernard Baduria

**SILVA**, Paulo Grane Gabriel Carcamo

## Compilation

### Linux

This project uses a [curses library](https://en.wikipedia.org/wiki/Curses_(programming_library)) for its user interface. Ensure you have the ncurses library installed:

**Red Hat/CentOS/Fedora**

```
sudo dnf install ncurses-devel
```

**Debian/Ubuntu**

```
sudo apt install libncurses5-dev libncursesw5-dev
```

**Arch**

```
sudo pacman -S ncurses
```

#### Compilation

Compile all source files in the `/src` directory and add `-lncurses` to the command to link the ncurses library. The command will look something like this.

```
g++ -g src/**.cpp -lncurses -o bin/a.out
```

### Windows

For Windows, we can use **PDCurses**, which is an implementation of curses that uses the same function calls as the popular **ncurses** for Unix systems. To compile using **MinGW64** (If you do not have it, I suggest installing it through [MSYS2](https://www.msys2.org/)):

**Manually building PDCurses**

1. [Download PDCurses 3.9](https://sourceforge.net/projects/pdcurses/files/pdcurses/3.9/PDCurses-3.9.zip/download) and extract contents.
2. In the `/wincon` directory, run `make -f Makefile WIDE=Y DLL=Y`.
3. This should have produced files, still in the `/wincon` directory:
    1. Copy the produced `pdcurses.dll` to `C:/msys64/mingw64/bin`.
    2. Copy the produced `pdcurses.a` to `C:/msys64/mingw64/lib`.
    3. Rename the `pdcurses.a` file you just copied to `libpdcurses.a`
    4. Go up one level from `/wincon` (i.e., the `PDCurses-3.9` folder). Copy `curses.h` and `panel.h` to `C:/msys64/mingw64/include`.

**Installing PDCurses through MSYS2 and `pacman`**
    
As an alternative to manually building PDCurses, you can install through MSYS2 like so:

```
pacman -S mingw-w64-x86_64-pdcurses
```

**However**, installing PDCurses through MSYS2 seems to result in the `curses.h` file *not* including the `endwin()` function for whatever reason. If you use this method of installation, **add the following to `main.cpp`**:

```c++
#include <pdcurses.h>
```
    
#### Compilation
    
Compile all source files in the `/src` directory and add `-lpdcurses` to the command to link the PDCurses library. The command will look something like this.

```
g++ -g src/**.cpp -o bin/a.exe -lpdcurses
```

If this does not work, explicitly include the `/include` and `lib` directories in the command.

```
g++ -g src\**.cpp -o bin\a.exe -IC:/msys64/mingw64/include -LC:/msys64/mingw64/lib -lpdcurses
```

Alternatively, use the `run.bat` file. This will automatically run the program as well.

```
run
```
> ***Warning***: Do ***not*** use the C/C++ extension and the built-in console for VSCode. It does not play nicely with curses.
