#include "grcs/composite.hpp"
#include "grcs/msg.hpp"

namespace grcs {

	bool Composite::isvalid() const {
		return Thesaurus::components.count(uuid);
	}

	void Composite::destroy() {
		if (!isvalid())
			return;

		Thesaurus::garbage.push_back(uuid);
	}

	void Thesaurus::init() {
		components_market = std::make_unique<lut::storage::Market>();
	}

	void Thesaurus::clear() {
		components_market.release();
	}

	Composite Thesaurus::createComposite() {
		lut::UUID uuid = lut::UUID::random();
		Composite ret(uuid);
		Thesaurus::components[uuid].clear();
		//Pigeon::send("composite-created", uuid);
		return ret;
	}

	const Composite Composite::Null;

	void Thesaurus::destroyComposite(Composite& composite) {
		for (auto& pair : components[composite.uuid])
			components_market->release(pair.first, pair.second);
		//Pigeon::send("composite-destroyed", composite.uuid);
		components.erase(composite.uuid);
		composite = Composite::Null;
	}

	void Thesaurus::clearGarbage() {
		for (const auto& uuid : garbage) {
			for (auto& pair : components[uuid])
				components_market->release(pair.first, pair.second);
			//Pigeon::send("composite-destroyed", uuid);
			components.erase(uuid);
		}
		garbage.clear();
	}

	void Thesaurus::updateComponents() {
		std::list<std::type_index> registered_types;
		for (auto& pair : components_market->shelves)
			registered_types.push_back(pair.first);

		registered_types.sort([&](const std::type_index& left, const std::type_index& right) {
			uint8_t prio_left = 0, prio_right = 0;
			if (priorities.count(left)) prio_left = priorities[left];
			if (priorities.count(right)) prio_right = priorities[right];
			return prio_left < prio_right;
		});

		for (const auto& type : registered_types) {
			//fprintf(stdout, "* updating components %s ...\n", type.name());
			components_market->foreach(type, [](void* item) {
				Component& component = *(Component*) item;
				if (!component.initialized) {
					component.oninit();
					component.initialized = true;
				}
				component.onupdate();
			});
			//fprintf(stdout, "* updated %lu components\n", components_market->shelves[type]->size());
		}
	}

	std::unique_ptr<lut::storage::Market> Thesaurus::components_market = nullptr;
	//std::unordered_map<lut::UUID, Composite> Thesaurus::composites;
	std::map<lut::UUID, std::unordered_map<std::type_index, lut::storage::handle_t>> Thesaurus::components;
	std::vector<lut::UUID> Thesaurus::garbage;
	std::unordered_map<std::type_index, uint8_t> Thesaurus::priorities;
	
}