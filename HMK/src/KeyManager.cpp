#include "KeyManager.h"

using namespace hmk;

bool KeyManager::keys_[512] = {};

bool KeyManager::get_key(unsigned short key)
{
	return keys_[key];
}

void KeyManager::set_key(unsigned short key, bool value)
{
	keys_[key] = value;
}