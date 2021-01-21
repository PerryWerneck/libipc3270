#!/bin/bash

install_packages() {

TEMPFILE=$(mktemp)

cat > ${TEMPFILE} << EOF
libtool
gettext-devel
cross-binutils
cross-gcc
cross-gcc-c++
cross-pkg-config
filesystem
lib3270-devel
libv3270-devel
EOF

# Instala apicativos e temas necessários
sudo zypper --non-interactive in \
        adwaita-icon-theme \
        gettext-tools \
        glib2-devel \
	autoconf \
	automake \
	libtool

while read FILE
do
	sudo zypper --non-interactive in ${1}-${FILE}
done < ${TEMPFILE}

rm -f ${TEMPFILE}

}

if [ -z ${1} ]; then
	echo "Use ${0} --32 for 32 bits cross-compiler"
	echo "Use ${0} --64 for 64 bits cross-compiler"
	exit -1
fi


until [ -z "${1}" ]
do
	if [ ${1:0:2} = '--' ]; then
		tmp=${1:2}
		parameter=${tmp%%=*}
		parameter=$(echo $parameter | tr "[:lower:]" "[:upper:]")

		case $parameter in

		ar)
			zypper ar obs://windows:mingw:win32 mingw32
			zypper ar obs://windows:mingw:win64 mingw64
			zypper ar obs://home:PerryWerneck:pw3270 pw3270
			;;

		32)
			install_packages mingw32
			;;

		64)
			install_packages mingw64
			;;

		ALL)
			install_packages mingw32
			install_packages mingw64
			;;


		*)
			value=${tmp##*=}
			eval $parameter=$value
		esac

	fi

	shift
done


