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

/**
 * @file
 *
 * @brief Implements linux request constructors based on TN3270::IPC::Session.
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include <config.h>
 #include <ipc-client-internals.h>
 #include "dbus-request.h"
 #include <string>
 #include <cstring>
 #include <algorithm>

 using namespace std;

 namespace TN3270 {

	DBus::Request::Request(DBusConnection *c, const char *id, const Type type, const char *name) : TN3270::Request{type}, connection{c} {

		if(!(id && *id)) {
			throw runtime_error("Empty request id");
		}

		debug("Session-id=",id);

		string object_name;
		string object_path{DBUS_OBJECT_PATH "/"};

		if(id[0] == ':') {

			object_name.assign(APPLICATION_ID ".");

			object_name += (id+1);
			object_path += (id+1);

		} else {

			const char *ptr = strchr(id,':');
			if(!ptr) {
				throw runtime_error("Invalid request id");
			}

			object_name = string{id, (size_t) (ptr-id) };
			object_path += (ptr+1);

		}

		std::transform(object_name.begin(), object_name.end(), object_name.begin(), ::tolower);
		std::transform(object_path.begin(), object_path.end(), object_path.begin(), ::tolower);

		debug("Object name=",object_name);
		debug("Object path=",object_path);

		static const char *interface_name = APPLICATION_ID ".session";

		switch(type) {
		case Request::Method:
			debug("Creating new method call '",name,"'");
			request.msg = dbus_message_new_method_call(
							object_name.c_str(),			// Destination
							object_path.c_str(),			// Path
							interface_name,					// Interface
							name							// Method
						);

			debug("Initializing dbus iter");
			dbus_message_iter_init_append(request.msg, &request.iter);

			break;

		case Request::GetProperty:
			debug("Creating new get property call '",name,"'");
			request.msg = dbus_message_new_method_call(
							object_name.c_str(),				// Destination
							object_path.c_str(),				// Path
							"org.freedesktop.DBus.Properties",	// Interface
							"Get"								// Method
						);

			debug("Initializing dbus iter");
			dbus_message_iter_init_append(request.msg, &request.iter);

			push(DBUS_TYPE_STRING,&interface_name);
			push(DBUS_TYPE_STRING,&name);

			break;

		case Request::SetProperty:
			debug("Creating new set property call '",name,"'");
			request.msg = dbus_message_new_method_call(
							object_name.c_str(),				// Destination
							object_path.c_str(),				// Path
							"org.freedesktop.DBus.Properties",	// Interface
							"Set"								// Method
						);

			debug("Initializing dbus iter");
			dbus_message_iter_init_append(request.msg, &request.iter);

			push(DBUS_TYPE_STRING,&interface_name);
			push(DBUS_TYPE_STRING,&name);

			request.variant = true;

			break;

		default:
			throw runtime_error("Unexpected request type");
		}

	}

	DBus::Request::~Request() {
		if(request.msg) {
			dbus_message_unref(request.msg);
		}
		if(response.msg) {
			dbus_message_unref(response.msg);
		}
	}

	Request & DBus::Request::call() {

		if(response.msg) {
			dbus_message_unref(response.msg);
			response.msg = nullptr;
		}

		DBusError error;
		dbus_error_init(&error);
		response.msg = dbus_connection_send_with_reply_and_block(this->connection,request.msg,10000,&error);

		if(!response.msg) {
			string message = error.message;
			dbus_error_free(&error);
			throw std::runtime_error(message);
		}

		dbus_message_iter_init(response.msg, &response.iter);

		return *this;

	}

	DBus::Request & DBus::Request::push(int type, const void *value) {

		if(request.variant) {

			// Is variant
			DBusMessageIter iter;
			char signature[] = { (char) type, 0 };

			if(!dbus_message_iter_open_container(&request.iter, DBUS_TYPE_VARIANT, signature, &iter)) {
				throw std::runtime_error("Can't open variant");
			}

			if(!dbus_message_iter_append_basic(&iter,type,value)) {
				dbus_message_iter_close_container(&request.iter, &iter);
				throw std::runtime_error("Can't append variant");
			}

			if (!dbus_message_iter_close_container(&request.iter, &iter)) {
				throw std::runtime_error("Can't close variant");
			}

			return *this;

		} else {

			// Basic type.
			if(dbus_message_iter_append_basic(&request.iter,type,value))
				return *this;

		}

		throw std::runtime_error("Can't append value");

	}

	TN3270::Request & DBus::Request::push(const char *arg) {
		return push(DBUS_TYPE_STRING,&arg);
	}

	TN3270::Request & DBus::Request::push(const bool arg) {
		dbus_bool_t bl = (arg ? 1 : 0);
		return push(DBUS_TYPE_BOOLEAN,&bl);
	}

	TN3270::Request & DBus::Request::push(const uint8_t arg) {
		return push(DBUS_TYPE_BYTE,&arg);
	}

	TN3270::Request & DBus::Request::push(const int32_t arg) {
		return push(DBUS_TYPE_INT32,&arg);
	}

	TN3270::Request & DBus::Request::push(const uint32_t arg) {
		return push(DBUS_TYPE_UINT32,&arg);
	}

	// Pop values
	TN3270::Request & DBus::Request::pop(std::string &value) {

		const char * str = "";

		if(dbus_message_iter_get_arg_type(&response.iter) == DBUS_TYPE_STRING) {

			dbus_message_iter_get_basic(&response.iter, &str);

		} else if(dbus_message_iter_get_arg_type(&response.iter) == DBUS_TYPE_VARIANT) {

			DBusMessageIter sub;
			int current_type;

			dbus_message_iter_recurse(&response.iter, &sub);

            while ((current_type = dbus_message_iter_get_arg_type(&sub)) != DBUS_TYPE_INVALID) {

                if (current_type == DBUS_TYPE_STRING) {
                    dbus_message_iter_get_basic(&sub, &str);
                    break;
                }
                dbus_message_iter_next(&sub);
            }

		} else {

			debug("Argument type is ", ((char) dbus_message_iter_get_arg_type(&response.iter)) );
			throw std::runtime_error("Expected an string data type");

		}

		dbus_message_iter_next(&response.iter);

		value.assign(str);
		debug("pop(",value,")");

		return *this;
	}

	TN3270::Request & DBus::Request::pop(int &value) {

		value = -1;

		if(dbus_message_iter_get_arg_type(&response.iter) == DBUS_TYPE_INT32) {

			dbus_int32_t rc = 0;
			dbus_message_iter_get_basic(&response.iter, &rc);
			value = rc;

		} else if(dbus_message_iter_get_arg_type(&response.iter) == DBUS_TYPE_INT16) {

			dbus_int16_t rc = 0;
			dbus_message_iter_get_basic(&response.iter, &rc);
			value = rc;

		} else if(dbus_message_iter_get_arg_type(&response.iter) == DBUS_TYPE_UINT32) {

			dbus_uint32_t rc = 0;
			dbus_message_iter_get_basic(&response.iter, &rc);
			value = rc;

		} else if(dbus_message_iter_get_arg_type(&response.iter) == DBUS_TYPE_INVALID) {

			throw std::runtime_error("Invalid data type");

		} else if(dbus_message_iter_get_arg_type(&response.iter) == DBUS_TYPE_VARIANT) {

			DBusMessageIter sub;
			int current_type = DBUS_TYPE_INVALID;

			dbus_message_iter_recurse(&response.iter, &sub);

			while ((current_type = dbus_message_iter_get_arg_type(&sub)) != DBUS_TYPE_INVALID) {

				debug("Current_type=",(char) current_type);

				if (current_type == DBUS_TYPE_INT32) {

					dbus_int32_t rc = 0;
					dbus_message_iter_get_basic(&sub, &rc);
					value = rc;
					break;

				} else if (current_type == DBUS_TYPE_UINT32) {

					dbus_uint32_t rc = 0;
					dbus_message_iter_get_basic(&sub, &rc);
					value = rc;
					break;

				} else if (current_type == DBUS_TYPE_INT16) {
					dbus_int16_t rc = 0;
					dbus_message_iter_get_basic(&sub, &rc);
					value = rc;
					break;

				}
				dbus_message_iter_next(&sub);
			}

			if(current_type == DBUS_TYPE_INVALID) {
				throw std::runtime_error("No integer data type in variant response");
			}

		} else {

			throw std::runtime_error("Expected an integer data type");

		}

		debug("pop(",value,")");
		return *this;

	}

	TN3270::Request & DBus::Request::pop(unsigned int &value) {

		value = (unsigned int) -1;

		if(dbus_message_iter_get_arg_type(&response.iter) == DBUS_TYPE_UINT32) {

			debug("DBUS_TYPE_UINT32");
			dbus_uint32_t rc = 0;
			dbus_message_iter_get_basic(&response.iter, &rc);
			value = rc;

		} else if(dbus_message_iter_get_arg_type(&response.iter) == DBUS_TYPE_UINT16) {

			debug("DBUS_TYPE_UINT16");
			dbus_uint16_t rc = 0;
			dbus_message_iter_get_basic(&response.iter, &rc);
			value = rc;

		} else if(dbus_message_iter_get_arg_type(&response.iter) == DBUS_TYPE_VARIANT) {

			debug("DBUS_TYPE_VARIANT");

			DBusMessageIter sub;
			int current_type = DBUS_TYPE_INVALID;

			dbus_message_iter_recurse(&response.iter, &sub);

			while ((current_type = dbus_message_iter_get_arg_type(&sub)) != DBUS_TYPE_INVALID) {

				if (current_type == DBUS_TYPE_UINT32) {

					dbus_uint32_t rc = 0;
					debug("DBUS_TYPE_VARIANT-DBUS_TYPE_UINT32");
					dbus_message_iter_get_basic(&sub, &rc);
					value = rc;
					break;

				} else if (current_type == DBUS_TYPE_UINT16) {
					dbus_uint16_t rc = 0;
					debug("DBUS_TYPE_VARIANT-DBUS_TYPE_UINT16");
					dbus_message_iter_get_basic(&sub, &rc);
					value = rc;
					break;

				} else if (current_type == DBUS_TYPE_INT16) {
					dbus_int16_t rc = 0;
					debug("DBUS_TYPE_VARIANT-DBUS_TYPE_UINT16");
					dbus_message_iter_get_basic(&sub, &rc);
					value = rc;
					break;

				} else if (current_type == DBUS_TYPE_INT32) {
					dbus_int32_t rc = 0;
					debug("DBUS_TYPE_VARIANT-DBUS_TYPE_INT32");
					dbus_message_iter_get_basic(&sub, &rc);
					value = rc;
					break;

				}
#ifdef DEBUG
				else {
					debug("Variant type is '",((char) current_type),"'");
				}
#endif // DEBUG

				dbus_message_iter_next(&sub);
			}

			if(current_type == DBUS_TYPE_INVALID) {
				throw std::runtime_error("No integer data type in variant response");
			}

		} else {

			throw std::runtime_error("Expected an integer data type");

		}

		debug("pop(",value,")");
		return *this;
	}

	TN3270::Request & DBus::Request::pop(bool &value) {

		value = false;

		if(dbus_message_iter_get_arg_type(&response.iter) == DBUS_TYPE_UINT32) {

			dbus_uint32_t rc = 0;
			dbus_message_iter_get_basic(&response.iter, &rc);
			value = (rc != 0);

		} else if(dbus_message_iter_get_arg_type(&response.iter) == DBUS_TYPE_UINT16) {

			dbus_uint16_t rc = 0;
			dbus_message_iter_get_basic(&response.iter, &rc);
			value = (rc != 0);

		} else if(dbus_message_iter_get_arg_type(&response.iter) == DBUS_TYPE_BOOLEAN) {

			dbus_bool_t rc = 0;
			dbus_message_iter_get_basic(&response.iter, &rc);
			value = (rc != 0);

		} else if(dbus_message_iter_get_arg_type(&response.iter) == DBUS_TYPE_VARIANT) {

			DBusMessageIter sub;
			int current_type = DBUS_TYPE_INVALID;

			dbus_message_iter_recurse(&response.iter, &sub);

			while ((current_type = dbus_message_iter_get_arg_type(&sub)) != DBUS_TYPE_INVALID) {

				if (current_type == DBUS_TYPE_UINT32) {

					dbus_uint32_t rc = 0;
					dbus_message_iter_get_basic(&sub, &rc);
					value = (rc != 0);
					break;

				} else if (current_type == DBUS_TYPE_UINT16) {

					dbus_uint16_t rc = 0;
					dbus_message_iter_get_basic(&sub, &rc);
					value = (rc != 0);
					break;

				} else if (current_type == DBUS_TYPE_BOOLEAN) {

					dbus_bool_t rc = 0;
					dbus_message_iter_get_basic(&sub, &rc);
					value = (rc != 0);
					break;

				}

				dbus_message_iter_next(&sub);

			}

			if(current_type == DBUS_TYPE_INVALID) {
				throw std::runtime_error("No boolean data type in variant response");
			}

		} else {

			throw std::runtime_error("Expected a boolean data type");

		}

		debug("pop(",value,")");
		return *this;
	}

 }


