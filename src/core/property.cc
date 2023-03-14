/*
 * "Software pw3270, desenvolvido com base nos códigos fontes do WC3270  e X3270
 * (Paul Mattes Paul.Mattes@usa.net), de emulação de terminal 3270 para acesso a
 * aplicativos mainframe. Registro no INPI sob o nome G3270.
 *
 * Copyright (C) <2008> <Banco do Brasil S.A.>
 *
 * Este programa é software livre. Você pode redistribuí-lo e/ou modificá-lo sob
 * os termos da GPL v.2 - Licença Pública Geral  GNU,  conforme  publicado  pela
 * Free Software Foundation.
 *
 * Este programa é distribuído na expectativa de  ser  útil,  mas  SEM  QUALQUER
 * GARANTIA; sem mesmo a garantia implícita de COMERCIALIZAÇÃO ou  de  ADEQUAÇÃO
 * A QUALQUER PROPÓSITO EM PARTICULAR. Consulte a Licença Pública Geral GNU para
 * obter mais detalhes.
 *
 * Você deve ter recebido uma cópia da Licença Pública Geral GNU junto com este
 * programa; se não, escreva para a Free Software Foundation, Inc., 51 Franklin
 * St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Este programa está nomeado como - e possui - linhas de código.
 *
 * Contatos:
 *
 * perry.werneck@gmail.com	(Alexandre Perry de Souza Werneck)
 * erico.mendonca@gmail.com	(Erico Mascarenhas Mendonça)
 *
 */

 #ifdef HAVE_CONFIG_H
	#include <config.h>
 #endif // HAVE_CONFIG_H

/**
 * @file src/os/linux/attribute.cc
 *
 * @brief Implements methods for static attribute management.
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include <lib3270/ipc.h>
 #include <lib3270/toggle.h>
 #include <lib3270/properties.h>
 #include <lib3270/ipc/property.h>

/*---[ Implement ]----------------------------------------------------------------------------------*/

 bool TN3270::Property::for_each(const std::function<bool(TN3270::Property &property)> &method) {

	class Property : public TN3270::Property {
	private:
		const LIB3270_PROPERTY &property;
		const Type attr_type;

	public:
		constexpr Property(const LIB3270_PROPERTY *p, const Type t) : property{*p},attr_type{t} {
		}

		Type type() const override {
			return attr_type;
		}

		const char * name() const {
			return property.name;
		}

		const char * description() const {
			return property.description;
		}

		const char * label() const {
			return property.label;
		}

		const char * icon() const {
			return property.icon;
		}

		const char * summary() const {
			return property.summary;
		}

	};

	for(auto prop = lib3270_get_int_properties_list(); prop->name; prop++) {
		Property attr{(const LIB3270_PROPERTY *) prop,Property::Integer};
		if(method(attr)) {
			return true;
		}
	}

	for(auto prop = lib3270_get_unsigned_properties_list(); prop->name; prop++) {
		Property attr{(const LIB3270_PROPERTY *) prop,Property::Unsigned};
		if(method(attr)) {
			return true;
		}
	}

	for(auto prop = lib3270_get_string_properties_list(); prop->name; prop++) {
		Property attr{(const LIB3270_PROPERTY *) prop,Property::String};
		if(method(attr)) {
			return true;
		}
	}

	for(auto prop = lib3270_get_toggles(); prop->name; prop++) {
		Property attr{(const LIB3270_PROPERTY *) prop,Property::Boolean};
		if(method(attr)) {
			return true;
		}
	}

	for(auto prop = lib3270_get_boolean_properties_list(); prop->name; prop++) {
		Property attr{(const LIB3270_PROPERTY *) prop,Property::Boolean};
		if(method(attr)) {
			return true;
		}
	}

 	return false;
 }

 TN3270_PUBLIC bool for_each(const std::function<bool(const LIB3270_PROPERTY &property)> &method) {

	for(auto prop = lib3270_get_int_properties_list(); prop->name; prop++) {
		if(method(*((const LIB3270_PROPERTY *) prop))) {
			return true;
		}
	}

	for(auto prop = lib3270_get_unsigned_properties_list(); prop->name; prop++) {
		if(method(*((const LIB3270_PROPERTY *) prop))) {
			return true;
		}
	}

	for(auto prop = lib3270_get_string_properties_list(); prop->name; prop++) {
		if(method(*((const LIB3270_PROPERTY *) prop))) {
			return true;
		}
	}

	for(auto prop = lib3270_get_toggles(); prop->name; prop++) {
		if(method(*((const LIB3270_PROPERTY *) prop))) {
			return true;
		}
	}

	for(auto prop = lib3270_get_boolean_properties_list(); prop->name; prop++) {
		if(method(*((const LIB3270_PROPERTY *) prop))) {
			return true;
		}
	}

	return false;

 }
