#!/bin/bash

. ./dbus.conf

gdbus \
	introspect \
	--session \
	--dest=${DBUS_DEST} \
	--object-path="${DBUS_PATH}"

