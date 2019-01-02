#!/bin/bash
gdbus \
	introspect \
	--session \
	--dest=br.com.bb.tn3270.service \
	--object-path=/br/com/bb/tn3270/service

