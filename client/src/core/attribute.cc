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
 * @file src/core/attribute.cc
 *
 * @brief Implements type independent property object.
 *
 * @author perry.werneck@gmail.com
 *
 */

 #include <ipc-client-internals.h>
 #include <string>
 #include <cstring>
 #include <stdexcept>

 using std::runtime_error;

/*---[ Implement ]----------------------------------------------------------------------------------*/

 namespace TN3270 {

	Attribute::~Attribute() {
	}

	std::string Attribute::getString() const {
		throw std::system_error(ENOTSUP, std::system_category());
	}

	int32_t Attribute::getInt32() const {
		throw std::system_error(ENOTSUP, std::system_category());
	}

	uint32_t Attribute::getUint32() const {
		throw std::system_error(ENOTSUP, std::system_category());
	}

	bool Attribute::getBool() const {
		throw std::system_error(ENOTSUP, std::system_category());
	}


	/*
	class StringProperty : public Property, std::string {
	public:
		StringProperty(const char *str) : Property(Property::String), std::string(str) {
		}

		StringProperty(const std::string &str) : Property(Property::String), std::string(str) {
		}

		std::string toString() const override {
			return std::string(this->c_str());
		}

		int32_t toInt32() const override {
			return (int32_t) atoi(this->c_str());
		}

		uint32_t toUint32() const override {
			return (uint32_t) atoi(this->c_str());
		}

		bool toBool() const override {
			return atoi(this->c_str()) != 0;
		}

	};

	Property::Property(Property::Type type) {
		this->type = type;

	}

	Property::~Property() {
	}

	std::string Property::toString() const {
		throw runtime_error("The value can't be converted to string");
	}

	int32_t Property::toInt32() const {
		throw runtime_error("The value can't be converted to a signed integer");
	}

	uint32_t Property::toUint32() const {
		throw runtime_error("The value can't be converted to an unsigned integer");
	}

	bool Property::toBool() const {
		throw runtime_error("The value can't be converted to boolean");
	}


	Property * Property::create(const char *str) {
		return new StringProperty(str);
	}

	Property * Property::create(const std::string &str) {
		return new StringProperty(str);
	}

	Property * Property::create(const int value) {

		class Value : public Property {
		private:
			int32_t value;

		public:
			Value(int value) : Property(Property::Int32) {
				this->value = (int32_t) value;
			}

			std::string toString() const override {
				return std::to_string(value);
			}

			int32_t toInt32() const override {
				return (int32_t) value;
			}

			uint32_t toUint32() const override {
				return (uint32_t) value;
			}

			bool toBool() const override {
				return value != 0;
			}

		};

		return new Value(value);

	}

	Property * Property::create(const unsigned int value) {

		class Value : public Property {
		private:
			uint32_t value;

		public:
			Value(unsigned int value) : Property(Property::Uint32) {
				this->value = (uint32_t) value;
			}

			std::string toString() const override {
				return std::to_string(value);
			}

			int32_t toInt32() const override {
				return (int32_t) value;
			}

			uint32_t toUint32() const override {
				return (uint32_t) value;
			}

			bool toBool() const override {
				return value != 0;
			}

		};

		return new Value(value);

	}

	Property * Property::create(const bool value) {

		class Value : public Property {
		private:
			bool value;

		public:
			Value(bool value) : Property(Property::Boolean) {
				this->value = value;
			}

			std::string toString() const override {
				return std::to_string(value);
			}

			int32_t toInt32() const override {
				return (int32_t) value;
			}

			uint32_t toUint32() const override {
				return (uint32_t) value;
			}

			bool toBool() const override {
				return value;
			}

		};

		return new Value(value);

	}
	*/


 }



