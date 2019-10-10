IPC (interprocess communication) plugin & library for pw3270.
=============================================================

Created originally as part of PW3270 application.

See more details at https://softwarepublico.gov.br/social/pw3270/

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

	* git clone https://github.com/PerryWerneck/libipc3270.git ./libipc3270

4. Build library using the mingw shell

	* cd libipc3270
	* ./autogen.sh
	* make all

5. Install

	* make install

