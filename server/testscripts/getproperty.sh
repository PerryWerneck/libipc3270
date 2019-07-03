#!/bin/bash
#
# https://stackoverflow.com/questions/48648952/set-get-property-using-dbus-send
#

dbus-send \
	--session \
	--dest=br.com.bb.pw3270.a\
	--print-reply \
	"/br/com/bb/tn3270/session" \
	"org.freedesktop.DBus.Properties.Get" \
	string:br.com.bb.tn3270.session \
	string:${1}

