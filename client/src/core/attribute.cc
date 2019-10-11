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

	Attribute::Attribute(const Attribute &src) {

		this->type = src.type;
		this->szData = src.szData;
		this->get = src.get;
		this->set = src.set;

		if(this->szData) {
			this->data = new uint8_t[this->szData];
			memcpy(this->data,src.data,this->szData);
		} else {
			this->data = nullptr;
		}

	}

	Attribute::Attribute(const Attribute *src) {

		this->type = src->type;
		this->szData = src->szData;

		this->get = src->get;
		this->set = src->set;

		if(this->szData) {
			this->data = new uint8_t[this->szData];
			memcpy(this->data,src->data,this->szData);
		} else {
			this->data = nullptr;
		}

	}

	Attribute::Attribute(Type type, size_t szWorker) {

		this->type = type;
		this->szData = szWorker;

		if(this->szData) {
			this->data = new uint8_t[this->szData];
			memset(this->data,0,this->szData);
		} else {
			this->data = nullptr;
		}

		debug("worker=",((void *) this->data)," length=",szWorker);

		get.name = [](const void GNUC_UNUSED(*worker)) {
			return "unnamed";
		};

		get.asString = [](const Attribute GNUC_UNUSED(&attr), const void GNUC_UNUSED(*worker)) {

			throw std::system_error(ENOTSUP, std::system_category());
			return "";
		};

		get.asInt32 = [](const Attribute GNUC_UNUSED(&attr), const void GNUC_UNUSED(*worker)) {
			throw std::system_error(ENOTSUP, std::system_category());
			return (int32_t) 0;
		};

		get.asUint32 = [](const Attribute &attr, const void *worker) {
			return (uint32_t) attr.get.asInt32(attr, worker);
		};

		get.asBoolean = [](const Attribute &attr, const void *worker) {
			return (bool) attr.get.asInt32(attr, worker) != 0;
		};

		set.asString = [](const Attribute GNUC_UNUSED(& attr), const void GNUC_UNUSED(*worker), const char GNUC_UNUSED(*value)) {
			throw std::system_error(ENOTSUP, std::system_category());
		};

		set.asInt32 = [](const Attribute GNUC_UNUSED(& attr), const void GNUC_UNUSED(*worker), const int32_t GNUC_UNUSED(value)) {
			throw std::system_error(ENOTSUP, std::system_category());
		};

		set.asUint32 = [](const Attribute & attr, const void *worker, const uint32_t value) {
			attr.set.asInt32(attr,worker,(int32_t) value);
		};

		set.asBoolean = [](const Attribute & attr, const void *worker, const bool value) {
			attr.set.asInt32(attr,worker,(int32_t) value);
		};

	}

	Attribute::~Attribute() {

		if(data) {
			delete[] data;
		}

	}

 }
