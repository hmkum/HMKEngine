#pragma once

namespace hmk
{
class KeyManager
{
public:
	static void set_key(unsigned short key, bool value);
	static bool get_key(unsigned short key);

private:
	static bool keys_[512];
};
}