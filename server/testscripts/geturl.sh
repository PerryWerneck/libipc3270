#!/bin/bash
#
# https://stackoverflow.com/questions/48648952/set-get-property-using-dbus-send
#

#dbus-send \
#	--session \
#	--dest=br.com.bb.pw3270.a\
#	--print-reply \
#	"/br/com/bb/tn3270/session" \
#	"org.freedesktop.DBus.Properties.Get" \
#	string:br.com.bb.tn3270.session \
#	string:url


gdbus \
	call \
	--session \
	--dest "br.com.bb.pw3270.a" \
	--object-path "/br/com/bb/tn3270/session" \
	--method org.freedesktop.DBus.Properties.Get \
	"br.com.bb.tn3270.session" \
	"url"

