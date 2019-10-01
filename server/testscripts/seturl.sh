#!/bin/bash
#
# https://stackoverflow.com/questions/48648952/set-get-property-using-dbus-send
#

dbus-send \
	--session \
	--dest=br.com.bb.pw3270.a\
	--print-reply \
	"/br/com/bb/tn3270/session" \
	"org.freedesktop.DBus.Properties.Set" \
	string:br.com.bb.tn3270.session \
	string:url \
	variant:string:${1}


#dbus-send --system --dest=$BUS_NAME --print-reply $OBJECT_PATH \
 #org.freedesktop.DBus.Properties.Set string:com.pgaur.GDBUS string:Status variant:uint32:10

#gdbus \
#        introspect \
#        --session \
#        --dest=br.com.bb.pw3270.a \
#        --object-path=/br/com/bb/tn3270/session

#gdbus \
#	call \
#	--session \
#	--dest br.com.bb.pw3270.a \
#	--object-path /br/com/bb/tn3270/session \
#	--method org.freedesktop.DBus.Properties.Set \
#	"br.com.bb.tn3270.session" \
#	"url" \
#	${1}



