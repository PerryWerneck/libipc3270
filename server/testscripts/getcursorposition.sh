#!/bin/bash
#
# https://stackoverflow.com/questions/48648952/set-get-property-using-dbus-send
#

. ./dbus.conf

dbus-send \
	--session \
	--dest=${DBUS_DEST} \
	--print-reply \
	"${DBUS_PATH}" \
	"${DBUS_INTERFACE}.getCursorPosition"

