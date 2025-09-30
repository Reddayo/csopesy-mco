# CSOPESY: Marquee console

**CESAR**, Jusper Angelo Manuel 

**LLOVIT**, Benn Erico Bernardo

**MARQUESES**, Lorenz Bernard Baduria

**SILVA**, Paulo Grane Gabriel Carcamo

## Linux users

This project uses a [curses library](https://en.wikipedia.org/wiki/Curses_(programming_library)) for its user interface. Ensure you have the ncurses library installed (as a note, this project was tested on Fedora):

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

### Compilation

Use the `run.sh` script located at the root directory.

```
run.sh
```

Alternatively, if you want to compile manually, compile all source files in the `/src` directory and add `-lncurses` to the command to link the ncurses library. The command will look something like this.

```
g++ -g src/**.cpp -lncurses -o bin/a.out
```

## Windows users

For Windows, we can use **PDCurses**, which is an implementation of curses that uses the same function calls as the popular **ncurses** for Unix systems. Compiling using GCC on **MinGW64** is strongly recommended. If you do not have it, I suggest installing it through [MSYS2](https://www.msys2.org/), using the package [`mingw-w64-x86_64-gcc`](https://packages.msys2.org/packages/mingw-w64-x86_64-gcc)):

```
pacman -S mingw-w64-x86_64-gcc
```

Ensure you also have access to `make`. Otherwise, you may also install it through MSYS2:

```
pacman -S base-devel
```

**Option 1: Using the build script**

Included is a batch file (`build_curses.bat`) to build PDCurses for you (hopefully).

1. [Download PDCurses 3.9](https://sourceforge.net/projects/pdcurses/files/pdcurses/3.9/PDCurses-3.9.zip/download) and extract contents.
2. Copy `build_pdcurses.bat` to the `PDCurses-3.9` directory (for reference, this should be the same directory as `README.md`) and execute it from there.
3. Supply the script with paths for your MinGW64 installation, or simply use the default values.

**Option 2: Manually building PDCurses**

The steps below are just what `build_curses.bat`, if you wish to do the steps manually for whatever reason.

1. [Download PDCurses 3.9](https://sourceforge.net/projects/pdcurses/files/pdcurses/3.9/PDCurses-3.9.zip/download) and extract contents.
2. In the `/wincon` directory, run `make -f Makefile WIDE=Y DLL=Y`.
3. This should have produced files, still in the `/wincon` directory:
    1. Copy the produced `pdcurses.dll` to `C:/msys64/mingw64/bin`.
    2. Copy the produced `pdcurses.a` to `C:/msys64/mingw64/lib`.
    3. Rename the `pdcurses.a` file you just copied to `libpdcurses.a`
    4. Go up one level from `/wincon` (i.e., the `PDCurses-3.9` folder). Copy `curses.h` and `panel.h` to `C:/msys64/mingw64/include`.

**Option 3: Installing PDCurses through MSYS2 and `pacman`**
 
As a last resort if all of the above fails, you can install PDCurses through MSYS2 like so:

```
pacman -S mingw-w64-x86_64-pdcurses
```

> [!WARNING]
> Installing PDCurses through MSYS2 will require that all instances of
>
> ```c++
> #include <curses.h>
> ```
>
> be replaced with
>
> ```c++
> #include <pdcurses.h>
> ```

### Compilation

Use the `run.bat` file located at the root directory

```
run
```

Alternatively, if you want to compile manually, compile all source files in the `/src` directory and add `-lpdcurses` to the command to link the PDCurses library. The command will look something like this.

```
g++ -g src/**.cpp -o bin/a.exe -lpdcurses
```

If this does not work, explicitly include the `/include` and `lib` directories in the command.

```
g++ -g src\**.cpp -o bin\a.exe -IC:/msys64/mingw64/include -LC:/msys64/mingw64/lib -lpdcurses
```

> [!WARNING]
> Do ***not*** use the C/C++ extension and the built-in console for VSCode. It does not play nicely with curses.
