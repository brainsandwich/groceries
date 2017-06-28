#pragma once

#include <LUT/storage.hpp>
#include <LUT/uuid.hpp>

#include <memory>
#include <unordered_map>
#include <typeindex>

namespace grcs {
	struct Thesaurus;

	struct Composite {
		lut::UUID uuid = lut::UUID::Null;
		std::unordered_map<std::type_index, lut::storage::handle_t> components;

		static const Composite Null;

		Composite() {}
		Composite(const lut::UUID& uuid)
			: uuid(uuid) {}

		template <typename T, typename ... Args>
		void attach(Args&& ... args) {
			auto tid = std::type_index(typeid(T));
			if (components.count(tid))
				return;

			components[tid] = Thesaurus::components_market->create<T>(std::forward<Args>(args) ...);
			get<T>().parent = this;
		}

		template <typename T>
		void detach() {
			auto tid = std::type_index(typeid(T));
			if (!components.count(tid))
				return;

			Thesaurus::components_market->release<T>(components[tid]);
			components.erase(tid);
		}

		template <typename T>
		T& get() {
			auto tid = std::type_index(typeid(T));
			return Thesaurus::components_market->get<T>(components[tid]);
		}

		template <typename T>
		const T& get() const {
			auto tid = std::type_index(typeid(T));
			return Thesaurus::components_market->get<T>(components[tid]);
		}

		template <typename T>
		bool contains() {
			auto tid = std::type_index(typeid(T));
			return components.count(tid) && Thesaurus::components_market->isvalid<T>(components[tid]);
		}
	};

	struct Thesaurus {
		static std::unique_ptr<lut::storage::Market> components_market;
		static std::unordered_map<lut::UUID, Composite> composites;

		static void init();
		static void clear();
		static Composite& createComposite();
		static void destroyComposite(Composite& composite);
	};

	struct Component {
		Composite* parent;
		virtual ~Component() {}
	};

}