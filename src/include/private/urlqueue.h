/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2024 Perry Werneck <perry.werneck@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

 /**
  * @brief Declares the URL Queue Agent.
  */

 #pragma once
 #include <udjat/defs.h>
 #include <udjat/tools/xml.h>
 #include <udjat/agent/sql.h>
 #include <udjat/tools/protocol.h>

 namespace Udjat {

	namespace SQL {

		class UDJAT_PRIVATE URLQueue : public SQL::Agent<size_t>, public Udjat::Protocol {
		private:

			/// @brief SQL Script to insert an URL on queue.
			const SQL::Script ins;

			/// @brief SQL Script to get fields for retry.
			const SQL::Script send;

			/// @brief SQL Script to remove URL sent from queue.
			const SQL::Script after_send;

			/// @brief Seconds to wait after a sucessfull send to send another queued row.
			time_t send_interval;

			/// @brief Interval to send after inserting url on queue.
			time_t send_delay;

			/// @brief Get State based on queue size.
			std::shared_ptr<Abstract::State> state() const;

		public:
			URLQueue(const XML::Node &node);
			virtual ~URLQueue();

			bool refresh(bool b) override;

			std::shared_ptr<Protocol::Worker> WorkerFactory() const override;
		};

	}

 }

