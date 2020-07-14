#!/bin/bash

PRODUCT_NAME=$(pkg-config --variable=product_name lib3270)

gdbus \
	introspect \
	--session \
	--dest=br.com.bb.${PRODUCT_NAME}.a \
	--object-path=/br/com/bb/${PRODUCT_NAME}/a

