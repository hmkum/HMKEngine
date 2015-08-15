#include "KeyManager.h"

using namespace hmk;

bool KeyManager::mKeys[512] = {};

bool KeyManager::GetKey(unsigned short key)
{
	return mKeys[key];
}

void KeyManager::SetKey(unsigned short key, bool value)
{
	mKeys[key] = value;
}