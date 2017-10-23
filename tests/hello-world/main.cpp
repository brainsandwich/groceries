#include <grcs/groceries.hpp>

struct Position : grcs::Component {
	double x = 0.0, y = 0.0;
	float rot = 0.0f;
};

#include <functional>

template <typename Signature>
struct delegate_t {
private:
	static uint32_t gid;
public:
	uint32_t id;
	std::function<Signature> function;
	delegate_t() : id(gid++) {}
	delegate_t(const std::function<Signature>& function)
		: id(gid++)
		, function(function) {}
};

template <typename Signature> uint32_t delegate_t<Signature>::gid = 0;

struct Input {
	enum class Key : uint8_t {
		Up, Left, Down, Right
	};

	enum class MouseButton : uint8_t {
		Left, Middle, Right
	};

	uint32_t mousex;
	uint32_t mousey;
	std::unordered_map<Key, bool> keys;
	std::unordered_map<MouseButton, bool> mousebuttons;

	std::unordered_map<Key, std::vector<delegate_t<void()>>> keycallbacks;
	std::unordered_map<MouseButton, std::vector<delegate_t<void()>>> mousecallbacks;
};

int main(int argc, char** argv) {
	Input input;
	getc(stdin);
}