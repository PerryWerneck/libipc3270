IPC (interprocess communication) plugin & library for pw3270.
=============================================================

Created originally as part of PW3270 application.

See more details at https://softwarepublico.gov.br/social/pw3270/

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
![CodeQL](https://github.com/PerryWerneck/lib3270/workflows/CodeQL/badge.svg)
![Analytics](https://ga-beacon.appspot.com/G-RSMGQ9Q5BG/github/libipc3270)
![Downloads](https://img.shields.io/github/downloads/PerryWerneck/libipc3270/total.svg)


Installation repositories
=========================

The latest version packaged for many linux distributions can be found in SuSE Build Service (https://build.opensuse.org/project/show/home:PerryWerneck:pw3270)

Requirements
============

 * GTK-3 (https://www.gtk.org/)
 * lib3270 (https://github.com/PerryWerneck/lib3270)
 * libv3270 (https://github.com/PerryWerneck/libv3270)


Building for Linux
==================


Cross-compiling for Windows
===========================

Cross-compiling on SuSE Linux (Native or WSL)
---------------------------------------------

1. First add the MinGW Repositories for your SuSE version from:

	* https://build.opensuse.org/project/show/windows:mingw:win32
	* https://build.opensuse.org/project/show/windows:mingw:win64
	* https://build.opensuse.org/project/show/home:PerryWerneck:pw3270
 
2. Get sources from git

	* git clone https://github.com/PerryWerneck/libipc3270.git ./libipc3270

3. Install cross compilers

3. Configure build

4. Build

	* cd libipc3270
	* make clean
	* make all


Compiling for Windows (With MSYS2)
----------------------------------

1. Build and install lib3270 

	* Follow the guide on https://github.com/PerryWerneck/lib3270

2. Build and install libv3270 

	* Follow the guide on https://github.com/PerryWerneck/libv3270

3. Get sources from git

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

Compiling for Windows (With MSVC)
---------------------------------

1. Install pw3270 with remote control and sdk modules

2. Install git for windows

3. Download and install Visual Studio Build Tools (https://visualstudio.microsoft.com/pt-br/downloads/)

4. Get sources from git

	```shell
	git clone https://github.com/PerryWerneck/libipc3270.git ./ipc3270
	```

5. Build and install

	```shell
	cd ipc3270
	install.bat
	```

