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

 using std::string;

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	IPC::Request::~Request() {
		if(msg.out) {
			dbus_message_unref(msg.out);
		}
		if(msg.in) {
			dbus_message_unref(msg.in);
		}
	}

	IPC::Request & IPC::Request::call() {

		if(msg.in) {
			dbus_message_unref(msg.in);
			msg.in = nullptr;
		}

		DBusError error;
		dbus_error_init(&error);
		this->msg.in = dbus_connection_send_with_reply_and_block(this->conn,this->msg.out,10000,&error);

		if(!this->msg.in) {
			string message = error.message;
			dbus_error_free(&error);
			throw std::runtime_error(message.c_str());
		}

		dbus_message_iter_init(msg.in, &msg.iter);

		debug(__FUNCTION__," got a valid response");

		return *this;

	}

	IPC::Request & IPC::Request::push(const char *arg) {
		dbus_message_append_args(this->msg.out,DBUS_TYPE_STRING,&arg,DBUS_TYPE_INVALID);
		return *this;
	}

	IPC::Request & IPC::Request::push(const bool arg) {
		dbus_message_append_args(this->msg.out,DBUS_TYPE_BOOLEAN,&arg,DBUS_TYPE_INVALID);
		return *this;
	}

	IPC::Request & IPC::Request::push(const uint8_t arg) {
		dbus_message_append_args(this->msg.out,DBUS_TYPE_BYTE,&arg,DBUS_TYPE_INVALID);
		return *this;
	}

	IPC::Request & IPC::Request::push(const int32_t arg) {
		dbus_message_append_args(this->msg.out,DBUS_TYPE_INT32,&arg,DBUS_TYPE_INVALID);
		return *this;
	}

	IPC::Request & IPC::Request::push(const uint32_t arg) {
		dbus_message_append_args(this->msg.out,DBUS_TYPE_UINT32,&arg,DBUS_TYPE_INVALID);
		return *this;
	}


	IPC::Request & IPC::Request::pop(std::string &value) {

		const char * str = "";

		if(dbus_message_iter_get_arg_type(&msg.iter) == DBUS_TYPE_STRING) {

			dbus_message_iter_get_basic(&msg.iter, &str);

		} else if(dbus_message_iter_get_arg_type(&msg.iter) == DBUS_TYPE_VARIANT) {

			DBusMessageIter sub;
			int current_type;

			dbus_message_iter_recurse(&msg.iter, &sub);

            while ((current_type = dbus_message_iter_get_arg_type(&sub)) != DBUS_TYPE_INVALID) {

                if (current_type == DBUS_TYPE_STRING) {
                    dbus_message_iter_get_basic(&sub, &str);
                    break;
                }
                dbus_message_iter_next(&sub);
            }

		} else {

			debug("Argument type is ", ((char) dbus_message_iter_get_arg_type(&msg.iter)) );
			throw std::runtime_error("Expected an string data type");

		}

		dbus_message_iter_next(&msg.iter);

		value.assign(str);

		debug(__FUNCTION__,"= \"",str,"\"");

		return *this;
	}

	static int getUIntValue(DBusMessageIter &iter) {

		if(dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_UINT32) {

			dbus_uint32_t rc = 0;
			dbus_message_iter_get_basic(&iter, &rc);
			return (int) rc;

		} else if(dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_UINT16) {

			dbus_uint16_t rc = 0;
			dbus_message_iter_get_basic(&iter, &rc);
			return (int) rc;

		} else if(dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_VARIANT) {

			DBusMessageIter sub;
			int current_type;

			dbus_message_iter_recurse(&iter, &sub);

			while ((current_type = dbus_message_iter_get_arg_type(&sub)) != DBUS_TYPE_INVALID) {

				if (current_type == DBUS_TYPE_UINT32) {

					dbus_uint32_t rc = 0;
					dbus_message_iter_get_basic(&sub, &rc);
					return (int) rc;

				} else if (current_type == DBUS_TYPE_UINT16) {
					dbus_uint16_t rc = 0;
					dbus_message_iter_get_basic(&sub, &rc);
					return (int) rc;

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

		debug("Argument type is ", ((char) dbus_message_iter_get_arg_type(&iter)) );
		throw std::runtime_error("Expected an integer data type");

	}

	IPC::Request & IPC::Request::Request::pop(int &value) {

		value = getIntValue(msg.iter);
		dbus_message_iter_next(&msg.iter);
		debug(__FUNCTION__,"= \"",value,"\"");

		return *this;

	}

	IPC::Request & IPC::Request::Request::pop(unsigned int &value) {

		value = getUIntValue(msg.iter);
		dbus_message_iter_next(&msg.iter);
		debug(__FUNCTION__,"= \"",value,"\"");

		return *this;

	}

 }


