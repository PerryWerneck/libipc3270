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
 * @file lib3270/ipc/request.h
 *
 * @brief Declares the TN3270 IPC Request object.
 *
 * @author perry.werneck@gmail.com
 *
 */

 #pragma once
 #include <lib3270/ipc.h>

 namespace TN3270 {

	/// @brief PW3270 IPC Request/Response.
	class TN3270_PUBLIC Request {
	public:

		enum Type : uint8_t {
			Method = 3,
			GetProperty = 1,
			SetProperty = 2
		};

		Request(const Type type = Method);
		virtual ~Request();

		/// @brief Call request, exception when failed.
		virtual Request & call() = 0;

		/// @brief Call request, get an string as response.
		std::string get_string();

		/// @brief Call request, get an int32 as response.
		int32_t get_int();

		/// @brief Call request, get response.
		template<typename T>
		inline T get() {
			T rc;
			call();
			pop(rc);
			return rc;
		}

		template<typename T>
		inline Request & get(T &value) {
			call();
			return pop(value);
		}

		// Push values
		Request & push();
		virtual Request & push(const char *arg) = 0;
		virtual Request & push(const bool arg) = 0;
		virtual Request & push(const int32_t arg) = 0;
		virtual Request & push(const uint32_t arg) = 0;
		virtual Request & push(const uint8_t arg) = 0;

		template<typename... Targs>
		Request & push(const char *str, Targs... Fargs) {
			push(str);
			return push(Fargs...);
		}

		template<typename... Targs>
		Request & push(const std::string &str, Targs... Fargs) {
			push(str.c_str());
			return push(Fargs...);
		}

		template<typename... Targs>
		Request & push(const bool value, Targs... Fargs) {
			push(value);
			return push(Fargs...);
		}

		template<typename... Targs>
		Request & push(const int32_t arg, Targs... Fargs) {
			push(arg);
			return push(Fargs...);
		}

		template<typename... Targs>
		Request & push(const uint32_t arg, Targs... Fargs) {
			push(arg);
			return push(Fargs...);
		}

		template<typename... Targs>
		Request & push(const uint8_t arg, Targs... Fargs) {
			push(arg);
			return push(Fargs...);
		}

		// Pop values
		Request & pop();
		virtual Request & pop(std::string &value) = 0;
		virtual Request & pop(int &value) = 0;
		virtual Request & pop(unsigned int &value) = 0;
		virtual Request & pop(bool &value) = 0;

		template<typename T, typename... Targs>
		Request & pop(T &value, Targs... Fargs) {
			pop(value);
			return pop(Fargs...);
		}
	};

	template <typename T>
	inline TN3270_PUBLIC Request & operator<<(Request & request, const T value) {
		return request.push(value);
	}

	template <typename T>
	inline TN3270_PUBLIC Request & operator>>(Request & request, const T value) {
		return request.pop(value);
	}

 }
