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
 * @file src/os/linux/request.cc
 *
 * @brief Implements linux request methods for remote session.
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include <ipc-client-internals.h>
 #include <cstring>

 using std::string;

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	IPC::Request::Request(DBusConnection * conn) {
		this->conn = conn;

		memset(&response,0,sizeof(response));
		memset(&request,0,sizeof(request));

	}

	IPC::Request::~Request() {
		if(request.msg) {
			dbus_message_unref(request.msg);
		}

		if(response.msg) {
			dbus_message_unref(response.msg);
		}
	}

	IPC::Request & IPC::Request::call() {

		if(response.msg) {
			dbus_message_unref(response.msg);
			response.msg = nullptr;
		}

		DBusError error;
		dbus_error_init(&error);
		response.msg = dbus_connection_send_with_reply_and_block(this->conn,request.msg,10000,&error);

		if(!response.msg) {
			string message = error.message;
			dbus_error_free(&error);
			throw std::runtime_error(message.c_str());
		}

		dbus_message_iter_init(response.msg, &response.iter);

//		debug(__FUNCTION__," got a valid response");

		return *this;

	}

	IPC::Request & IPC::Request::push(int type, const void *value) {

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

	IPC::Request & IPC::Request::push(const char *arg) {
		return push(DBUS_TYPE_STRING,&arg);
	}

	IPC::Request & IPC::Request::push(const bool arg) {
		return push(DBUS_TYPE_BOOLEAN,&arg);
	}

	IPC::Request & IPC::Request::push(const uint8_t arg) {
		return push(DBUS_TYPE_BYTE,&arg);
	}

	IPC::Request & IPC::Request::push(const int32_t arg) {
		return push(DBUS_TYPE_INT32,&arg);
	}

	IPC::Request & IPC::Request::push(const uint32_t arg) {
		return push(DBUS_TYPE_UINT32,&arg);
	}

	IPC::Request & IPC::Request::pop(std::string &value) {

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

		debug(__FUNCTION__,"= \"",str,"\"");

		return *this;
	}

	static unsigned int getUIntValue(DBusMessageIter &iter) {

		if(dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_UINT32) {

			dbus_uint32_t rc = 0;
			dbus_message_iter_get_basic(&iter, &rc);
			return (unsigned int) rc;

		} else if(dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_UINT16) {

			dbus_uint16_t rc = 0;
			dbus_message_iter_get_basic(&iter, &rc);
			return (unsigned int) rc;

		} else if(dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_VARIANT) {

			DBusMessageIter sub;
			int current_type;

			dbus_message_iter_recurse(&iter, &sub);

			while ((current_type = dbus_message_iter_get_arg_type(&sub)) != DBUS_TYPE_INVALID) {

				if (current_type == DBUS_TYPE_UINT32) {

					dbus_uint32_t rc = 0;
					dbus_message_iter_get_basic(&sub, &rc);
					return (unsigned int) rc;

				} else if (current_type == DBUS_TYPE_UINT16) {
					dbus_uint16_t rc = 0;
					dbus_message_iter_get_basic(&sub, &rc);
					return (unsigned int) rc;

				}

				dbus_message_iter_next(&sub);
			}

		}

		debug("Argument type is ", ((char) dbus_message_iter_get_arg_type(&iter)) );
		throw std::runtime_error("Expected an integer data type");

	}

	static unsigned int getBooleanValue(DBusMessageIter &iter) {

		if(dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_UINT32) {

			dbus_uint32_t rc = 0;
			dbus_message_iter_get_basic(&iter, &rc);
			return rc != 0;

		} else if(dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_UINT16) {

			dbus_uint16_t rc = 0;
			dbus_message_iter_get_basic(&iter, &rc);
			return rc != 0;

		} else if(dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_BOOLEAN) {

			dbus_bool_t rc = 0;
			dbus_message_iter_get_basic(&iter, &rc);
			return rc;

		} else if(dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_VARIANT) {

			DBusMessageIter sub;
			int current_type;

			dbus_message_iter_recurse(&iter, &sub);

			while ((current_type = dbus_message_iter_get_arg_type(&sub)) != DBUS_TYPE_INVALID) {

				if (current_type == DBUS_TYPE_UINT32) {

					dbus_uint32_t rc = 0;
					dbus_message_iter_get_basic(&sub, &rc);
					return rc != 0;

				} else if (current_type == DBUS_TYPE_UINT16) {

					dbus_uint16_t rc = 0;
					dbus_message_iter_get_basic(&sub, &rc);
					return rc != 0;

				} else if (current_type == DBUS_TYPE_BOOLEAN) {

					dbus_bool_t rc = 0;
					dbus_message_iter_get_basic(&sub, &rc);
					return rc;

				}

				dbus_message_iter_next(&sub);
			}

		}

		debug("Argument type is ", ((char) dbus_message_iter_get_arg_type(&iter)) );
		throw std::runtime_error("Expected an integer data type");

	}

	static int getIntValue(DBusMessageIter &iter) {

		if(dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_INT32) {

			dbus_int32_t rc = 0;
			dbus_message_iter_get_basic(&iter, &rc);
			return (int) rc;

		} else if(dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_INT16) {

			dbus_int16_t rc = 0;
			dbus_message_iter_get_basic(&iter, &rc);
			return (int) rc;

		} else if(dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_VARIANT) {

			DBusMessageIter sub;
			int current_type;

			dbus_message_iter_recurse(&iter, &sub);

			while ((current_type = dbus_message_iter_get_arg_type(&sub)) != DBUS_TYPE_INVALID) {

				debug("Current_type=",(char) current_type);

				if (current_type == DBUS_TYPE_INT32) {

					dbus_int32_t rc = 0;
					dbus_message_iter_get_basic(&sub, &rc);
					return (int) rc;

				} else if (current_type == DBUS_TYPE_INT16) {
					dbus_int16_t rc = 0;
					dbus_message_iter_get_basic(&sub, &rc);
					return (int) rc;

				}
				dbus_message_iter_next(&sub);
			}

		}

		debug("Argument type is %d", ((int) dbus_message_iter_get_arg_type(&iter)) );
		throw std::runtime_error("Expected an integer data type");

	}

	IPC::Request & IPC::Request::Request::pop(int &value) {

		value = getIntValue(response.iter);
		dbus_message_iter_next(&response.iter);
//		debug(__FUNCTION__,"= \"",value,"\"");

		return *this;

	}

	IPC::Request & IPC::Request::Request::pop(unsigned int &value) {

		value = getUIntValue(response.iter);
		dbus_message_iter_next(&response.iter);
//		debug(__FUNCTION__,"= \"",value,"\"");

		return *this;

	}

	IPC::Request & IPC::Request::Request::pop(bool &value) {

		value = getBooleanValue(response.iter);
		dbus_message_iter_next(&response.iter);
//		debug(__FUNCTION__,"= \"",value,"\"");

		return *this;

	}

 }


