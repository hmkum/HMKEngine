#pragma once

namespace hmk
{
class KeyManager
{
public:
	static void SetKey(unsigned short key, bool value);
	static bool GetKey(unsigned short key);

private:
	static bool mKeys[512];
};
}