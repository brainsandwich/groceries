#include "grcs/composite.hpp"
#include "grcs/msg.hpp"

namespace grcs {

	void Thesaurus::init() {
		components_market = std::make_unique<lut::storage::Market>();
	}

	void Thesaurus::clear() {
		components_market.release();
	}

	Composite& Thesaurus::createComposite() {
		lut::UUID uuid = lut::UUID::random();
		composites[uuid] = Composite(uuid, components_market.get());
		Pigeon::send("composite-created", uuid);
		return composites[uuid];
	}

	void Thesaurus::destroyComposite(Composite& composite) {
		for (auto& pair : composite.components)
			components_market->release(pair.first, pair.second);
	}

	std::unique_ptr<lut::storage::Market> Thesaurus::components_market = nullptr;
	std::unordered_map<lut::UUID, Composite> Thesaurus::composites;
	
}