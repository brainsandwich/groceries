#pragma once

#include <cstdint>
#include <unordered_map>
#include <deque>
#include <list>
#include <functional>

namespace grcs {

	struct Packet {
		uint8_t* data = nullptr;
		std::size_t size = 0;

		Packet() {}

		template <typename T>
		Packet(const T& value) {
			size = sizeof(T);
			data = new uint8_t[size];
			memcpy(data, &value, size);
		}

		template <typename T> T& unpack() const {
			return *reinterpret_cast<T*>(data);
		}

		void clear() {
			delete[] data;
		}
	};

	struct Receiver {
		std::string type;
		std::function<void(const Packet&)> onmessage;

		Receiver() {}

		template <typename Func>
		Receiver(const std::string& type, const Func& onmessage)
			: type(type)
			, onmessage(onmessage) {}
	};

	struct Pigeon {
		static std::unordered_map<std::string, std::deque<Packet>> messages;
		static std::unordered_map<std::string, std::list<const Receiver*>> receivers;

		static void registerReceiver(const Receiver* receiver);
		static void unregisterReceiver(const Receiver* receiver);
		static void send(const std::string& message);
		static void dispatch();

		template <typename T>
		static void send(const std::string& message, const T& data) {
			messages[message].push_back(Packet(data));
		}
	};
}