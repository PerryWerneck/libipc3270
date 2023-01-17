#!/bin/bash
#
# https://stackoverflow.com/questions/48648952/set-get-property-using-dbus-send
#

dbus-send \
	--session \
	--dest=br.com.bb.pw3270.a\
	--print-reply \
	"/br/com/bb/tn3270/session" \
	"br.com.bb.tn3270.session.getStringAtAddress" \
	int32:0 \
	int32:-1 \
	byte:0

