## IPC (interprocess communication) library for pw3270.

Created originally as part of PW3270 application this library is designed to act as a standard ABI for lib320 and pw3270 language bindings using the same method and objects for direct lib3270 calls and IPC based requests do the pw3270 GUI application.

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
![Downloads](https://img.shields.io/github/downloads/PerryWerneck/libipc3270/total.svg)
[![build result](https://build.opensuse.org/projects/home:PerryWerneck:pw3270/packages/libipc3270/badge.svg?type=percent)](https://build.opensuse.org/package/show/home:PerryWerneck:pw3270/libipc3270)

## Instalation

### Linux

For the supported distributions get the install repositories and instructions from https://software.opensuse.org/download.html?project=home%3APerryWerneck%3Apw3270&package=libipc3270

## Building for Linux

### Requirements

 * [lib3270](../../../lib3270)

### Building

 * TODO

## Building for windows

### Cross-compiling on SuSE Linux (Native or WSL)

1. Add the MinGW Repositories

	```
	sudo zypper ar obs://windows:mingw:win32 mingw32
	sudo zypper ar obs://windows:mingw:win64 mingw64
	sudo zypper ar obs://home:PerryWerneck:pw3270 pw3270
	sudo zypper ref
	```
2. Get sources from git

	```shell
	git clone https://github.com/PerryWerneck/libipc3270.git ./libipc3270
	```

3. Install 64 bits cross compilers

	```shell
	./libipc3270/win/install-cross.sh --64
	```

3. Configure 64 bits build environment

        ```shell
        ./libipc3270/win/win-configure.sh --64
        ```

4. Build

	```shell
	cd libipc3270
	make clean
	make all
	```

### Windows native with MSYS2

1. Build and install [lib3270](../../../lib3270)

2. Get sources from git

	```shell
	git clone https://github.com/PerryWerneck/libipc3270.git ./libipc3270
	```

4. Build library using the mingw shell

	```shell
	cd libipc3270
	./autogen.sh
	make all
	```
5. Install

	```shell
	make install
	```

### Windows native with MSVC

**OUTDATED**

1. Install pw3270 with sdk modules

2. Install git for windows

3. Download and install [Visual Studio Build Tools](https://visualstudio.microsoft.com/pt-br/downloads/)

4. Get sources from git

	```shell
	git clone https://github.com/PerryWerneck/libipc3270.git ./ipc3270
	```

5. Build and install

	```shell
	cd ipc3270
	install.bat
	```

