#include "grcs/msg.hpp"

namespace grcs {

	void Pigeon::registerReceiver(const Receiver* receiver) {
		receivers[receiver->type].push_back(receiver);
	}

	void Pigeon::unregisterReceiver(const Receiver* receiver) {
		receivers[receiver->type].remove(receiver);
	}

	void Pigeon::send(const std::string& message) {
		messages[message].push_back(Packet(nullptr));
	}

	void Pigeon::dispatch() {
		for (auto& pair : messages) {
			for (const auto& msg : pair.second) {
				for (const auto& recv : receivers[pair.first])
					recv->onmessage(msg);
			}

			for (auto& msg : pair.second)
				msg.clear();

			pair.second.resize(0);
		}
	}

	std::unordered_map<std::string, std::deque<Packet>> Pigeon::messages;
	std::unordered_map<std::string, std::list<const Receiver*>> Pigeon::receivers;
}