#pragma once

#include <LUT/storage.hpp>
#include <LUT/uuid.hpp>

#include <memory>
#include <unordered_map>
#include <map>
#include <typeindex>

namespace grcs {
	struct Thesaurus;

	struct Composite {
		lut::UUID uuid = lut::UUID::Null;
		static const Composite Null;

		Composite() {}
		Composite(const lut::UUID& uuid)
			: uuid(uuid) {}

		bool isvalid() const;
		void destroy();

		template <typename T, typename ... Args> void attach(Args&& ... args);
		template <typename T> void detach();
		template <typename T> T& get();
		template <typename T> const T& get() const;
		template <typename T> bool contains() const;
	};

	struct Component {
		bool initialized = false;
		bool enabled = true;
		Composite parent = Composite::Null;

		virtual ~Component() {}
		virtual void oninit() {}
		virtual void onupdate() {}
		virtual void ondestroy() {}
	};

	struct Thesaurus {
		static std::unique_ptr<lut::storage::Market> components_market;
		static std::unordered_map<std::type_index, uint8_t> priorities;
		static std::map<lut::UUID, std::unordered_map<std::type_index, lut::storage::handle_t>> components;
		static std::vector<lut::UUID> garbage;

		static void init();
		static void clear();
		static Composite createComposite();
		static void destroyComposite(Composite& composite);
		static void clearGarbage();
		template <typename Func> static void foreach(const Func& func) {
			for (const auto& cmp : components)
				func(Composite(cmp.first));
		}

		static void updateComponents();
	};

	template <typename T, typename ... Args>
	void Composite::attach(Args&& ... args) {
		auto tid = std::type_index(typeid(T));
		if (Thesaurus::components[uuid].count(tid))
			return;

		Thesaurus::components[uuid][tid] = Thesaurus::components_market->create<T>(std::forward<Args>(args) ...);
		get<T>().parent = *this;
	}


	template <typename T>
	void Composite::detach() {
		if (!Thesaurus::components.count(uuid))
			throw std::exception(std::string("composite " + uuid.tostring() + " does not exist in thesaurus").c_str());

		auto tid = std::type_index(typeid(T));
		if (!components.count(tid))
			return;

		Thesaurus::components_market->release<T>(components[tid]);
		components.erase(tid);
	}

	template <typename T>
	T& Composite::get() {
		if (!Thesaurus::components.count(uuid))
			throw std::exception(std::string("composite " + uuid.tostring() + " does not exist in thesaurus").c_str());

		auto tid = std::type_index(typeid(T));
		if (!Thesaurus::components[uuid].count(tid))
			throw std::exception(std::string("no component of type " + std::string(tid.name()) + " belongs to composite " + uuid.tostring()).c_str());

		return Thesaurus::components_market->get<T>(Thesaurus::components[uuid][tid]);
	}

	template <typename T>
	const T& Composite::get() const {
		if (!Thesaurus::components.count(uuid))
			throw std::exception(std::string("composite " + uuid.tostring() + " does not exist in thesaurus").c_str());

		auto tid = std::type_index(typeid(T));
		if (!Thesaurus::components[uuid].count(tid))
			throw std::exception(std::string("no component of type " + std::string(tid.name()) + " belongs to composite " + uuid.tostring()).c_str());

		return Thesaurus::components_market->get<T>(Thesaurus::components[uuid][tid]);
	}

	template <typename T>
	bool Composite::contains() const {
		if (!Thesaurus::components.count(uuid))
			throw std::exception(std::string("composite " + uuid.tostring() + " does not exist in thesaurus").c_str());

		auto tid = std::type_index(typeid(T));
		return Thesaurus::components[uuid].count(tid) && Thesaurus::components_market->isvalid<T>(Thesaurus::components[uuid][tid]);
	}

}