#!/bin/bash
gdbus \
	introspect \
	--session \
	--dest=br.com.bb.$(pkg-config --variable=product_name lib3270).a \
	--object-path=/br/com/bb/tn3270/session

