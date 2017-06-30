#include <grcs/groceries.hpp>
#include <LUT/utility.hpp>

struct Position : grcs::Component {
	double x = 0.0, y = 0.0;
	float z = 0.0f;
	float rot = 0.0f;
};

struct UnitProperties {
	uint32_t health = 0;
	uint32_t energy = 0;
	uint32_t armor = 0;
	uint32_t strength = 0;
	float speed = 0;
	float health_regen = 0.0f;
	float energy_regen = 0.0f;

	UnitProperties& operator+=(const UnitProperties& other) {
		health += other.health;
		energy += other.energy;
		armor += other.armor;
		strength += other.strength;
		speed += other.speed;
		health_regen += other.health_regen;
		energy_regen += other.energy_regen;
		return *this;
	}

	UnitProperties operator+(const UnitProperties& other) {
		UnitProperties result;
		result.health = health + other.health;
		result.energy = energy + other.energy;
		result.armor = armor + other.armor;
		result.strength = strength + other.strength;
		result.speed = speed + other.speed;
		result.health_regen = health_regen + other.health_regen;
		result.energy_regen = energy_regen + other.energy_regen;
		return result;
	}
};

struct UnitSheet {
	std::string name = "";
	std::string desc = "";
	float size = 1.0f;
	UnitProperties properties;
};

struct WeaponType {
	static const uint32_t Instant = 0x0;
	static const uint32_t Projectile = 0x1;
	static const uint32_t Multi = 0x1 << 1;
};

struct WeaponProperties {
	uint32_t damage = 0;
	float distance = 0.0f;

	WeaponProperties operator+(const WeaponProperties& other) {
		WeaponProperties result;
		result.damage = damage + other.damage;
		result.distance = distance + other.distance;
		return result;
	}

	WeaponProperties& operator+=(const WeaponProperties& other) {
		damage += other.damage;
		distance += other.distance;
		return *this;
	}
};

struct WeaponSheet {
	std::string name = "";
	std::string desc = "";
	uint32_t flags = WeaponType::Instant;
	WeaponProperties properties;
};

struct ItemType {
	static const uint32_t Passive = 0x0;		// only target Caster
	static const uint32_t Active = 0x1;
	static const uint32_t Consumable = 0x1 << 1;
};

struct ItemTarget {
	static const uint32_t Caster = 0x1 << 2;	// only type Passive
	static const uint32_t Target = 0x1 << 3;
	static const uint32_t Area = 0x1 << 4;
	static const uint32_t CasterArea = 0x1 << 5;
};

struct ItemSheet {
	std::string name = "";
	std::string desc = "";
	uint32_t flags = ItemType::Passive | ItemTarget::Caster;
	uint32_t count = 0;		// consumable
	float duration = 0.0f;	// !passive

	UnitProperties unit_modifiers;
	WeaponProperties weapon_modifiers;
};

struct Unit : grcs::Component {
	// formal properties
	UnitSheet* sheet = nullptr;
	std::vector<UnitProperties> modifiers;
	std::vector<ItemSheet*> items;
	WeaponSheet* weapon = nullptr;
	bool properties_changed = true;

	// temporary properties
	UnitProperties unit_properties;
	WeaponProperties weapon_properties;

	uint32_t current_health = 0;
	float sub_health = 0.0f;
	uint32_t current_energy = 0;
	float sub_energy = 0.0f;

	void update(double dt) {
		if (!sheet)
			return;

		if (properties_changed) {
			UnitProperties combined_up = sheet->properties;
			WeaponProperties combined_wp = weapon != nullptr ? weapon->properties : WeaponProperties();
			for (const auto& mod : modifiers)
				combined_up += mod;

			for (const auto& item : items) {
				combined_up += item->unit_modifiers;
				combined_wp += item->weapon_modifiers;
			}

			unit_properties = combined_up;
			weapon_properties = combined_wp;

			current_health = unit_properties.health;
			current_energy = unit_properties.energy;
			properties_changed = false;
		}
		else {
			if (current_health < unit_properties.health) {
				sub_health += unit_properties.health_regen * dt;
				if (sub_health >= 1.0f) {
					float integral = 0.0f;
					sub_health = std::modf(sub_health, &integral);
					current_health += std::max(unit_properties.health, current_health + (uint32_t) integral);
				}
			}

			if (current_energy < unit_properties.energy) {
				sub_energy += unit_properties.energy_regen * dt;
				if (sub_energy >= 1.0f) {
					float integral = 0.0f;
					sub_energy = std::modf(sub_energy, &integral);
					current_energy += std::max(unit_properties.energy, current_energy + (uint32_t) integral);
				}
			}
		}
	}
};

int main(int argc, char** argv) {
	grcs::Thesaurus::init();

	auto& cmp = grcs::Thesaurus::createComposite();
	cmp.attach<Position>();
	cmp.attach<Unit>();

	WeaponSheet sword;
	sword.name = "Longsword";
	sword.desc = "Classic vintage sword";
	sword.flags = WeaponType::Instant;
	sword.properties.distance = 1.5f;
	sword.properties.damage = 27;

	WeaponSheet bare;
	bare.name = "No weapon";
	bare.desc = "This unit uses any kind of limb to fight";
	bare.flags = WeaponType::Instant;
	bare.properties.distance = 1.5f;
	bare.properties.damage = 0;

	ItemSheet gauntlets;
	gauntlets.name = "Leather Gauntlets";
	gauntlets.desc = "Classic vintage signature gauntlets";
	gauntlets.unit_modifiers.armor = 2;

	UnitSheet human;
	human.name = "Human";
	human.desc = "Fleshy bipedal sentient creature";
	human.properties.health = 250;
	human.properties.health_regen = 0.25f;
	human.properties.energy = 100;
	human.properties.energy_regen = 0.75f;
	human.properties.strength = 5;
	human.properties.speed = 2.5f;

	auto& unit = cmp.get<Unit>();
	unit.weapon = &sword;
	unit.items.push_back(&gauntlets);
	unit.sheet = &human;

	lut::w8usr();
	return 0;
}