// stdafx.cpp : source file that includes just the standard includes
//	UserInterface.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#ifdef __AUTO_HUNT__
float GetDistanceNew(const TPixelPosition& PixelPosition, const TPixelPosition& c_rPixelPosition)
{
	const float fdx = PixelPosition.x - c_rPixelPosition.x;
	const float fdy = PixelPosition.y - c_rPixelPosition.y;
	return sqrtf((fdx * fdx) + (fdy * fdy));
}

#include <boost/algorithm/string/classification.hpp> // Include boost::for is_any_of
#include <boost/algorithm/string/split.hpp>
void split_argument(const char* argument, std::vector<std::string>& vecArgs, const char* arg)
{
	boost::split(vecArgs, argument, boost::is_any_of(arg), boost::token_compress_on);
}
#endif

#ifdef __CAPTCHA__
static std::string base64_encode(const std::string& in) {

	std::string out;

	int val = 0, valb = -6;
	for (int jj = 0; jj < in.size(); jj++) {
		char c = in[jj];
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0) {
			out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}
	if (valb > -6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
	while (out.size() % 4) out.push_back('=');
	return out;
}

static std::string base64_decode(const std::string& in) {

	std::string out;

	std::vector<int> T(256, -1);
	for (int i = 0; i < 64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

	int val = 0, valb = -8;
	for (int jj = 0; jj < in.size(); jj++) {
		char c = in[jj];
		if (T[c] == -1) break;
		val = (val << 6) + T[c];
		valb += 6;
		if (valb >= 0) {
			out.push_back(char((val >> valb) & 0xFF));
			valb -= 8;
		}
	}
	return out;
}

std::string AVAILABLE_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
int index(char c) {
	for (int ii = 0; ii < AVAILABLE_CHARS.size(); ii++) {
		if (AVAILABLE_CHARS[ii] == c) {
			// std::cout << ii << " " << c << std::endl;
			return ii;
		}
	}
	return -1;
}

std::string extend_key(std::string& msg, std::string& key) {
	//generating new key
	int msgLen = msg.size();
	std::string newKey(msgLen, 'x');
	int keyLen = key.size(), i, j;
	for (i = 0, j = 0; i < msgLen; ++i, ++j) {
		if (j == keyLen)
			j = 0;

		newKey[i] = key[j];
	}
	newKey[i] = '\0';
	return newKey;
}

std::string encrypt_vigenere(std::string& msg, std::string& key) {
	int msgLen = msg.size(), keyLen = key.size(), i, j;
	std::string encryptedMsg(msgLen, 'x');
	// char newKey[msgLen], encryptedMsg[msgLen], decryptedMsg[msgLen];

	std::string newKey = extend_key(msg, key);

	//encryption
	for (i = 0; i < msgLen; ++i) {
		// std::cout << msg[i] << " " << isalnum(msg[i]) << std::endl;
		if (isalnum(msg[i]) || msg[i] == ' ') {
			encryptedMsg[i] = AVAILABLE_CHARS[((index(msg[i]) + index(newKey[i])) % AVAILABLE_CHARS.size())];
		}
		else {
			encryptedMsg[i] = msg[i];
		}
	}

	encryptedMsg[i] = '\0';
	return encryptedMsg;
}

std::string decrypt_vigenere(std::string& encryptedMsg, std::string& newKey) {
	// decryption
	int msgLen = encryptedMsg.size();
	std::string decryptedMsg(msgLen, 'x');
	int i;
	for (i = 0; i < msgLen; ++i) {
		if (isalnum(encryptedMsg[i]) || encryptedMsg[i] == ' ') {
			decryptedMsg[i] = AVAILABLE_CHARS[(((index(encryptedMsg[i]) - index(newKey[i])) + AVAILABLE_CHARS.size()) % AVAILABLE_CHARS.size())];
		}
		else {
			decryptedMsg[i] = encryptedMsg[i];
		}
	}
	decryptedMsg[i] = '\0';
	return decryptedMsg;
}

std::string encrypt(std::string& msg, std::string key) {
	std::string b64_str = base64_encode(msg);
	const std::string vigenere_msg = encrypt_vigenere(b64_str, key);
	// std::cout << vigenere_msg << std::endl;
	return vigenere_msg;
}

std::string decrypt(std::string& encrypted_msg, std::string key) {
	std::string newKey = extend_key(encrypted_msg, key);
	const std::string b64_encoded_str = decrypt_vigenere(encrypted_msg, newKey);
	const std::string b64_decode_str = base64_decode(b64_encoded_str);
	return b64_decode_str;
}
#endif

#ifdef ENABLE_DUNGEON_BOSS_ICON_IN_MAP
#include "PythonNonPlayer.h"
bool IsBoss(DWORD race)
{
	const CPythonNonPlayer::TMobTable* pMobTable = CPythonNonPlayer::Instance().GetTable(race);
	if (pMobTable && pMobTable->bType  == 0 && pMobTable->bRank >= 4)
		return true;
	return false;
}
bool GetDungeonNPC(DWORD mobIndex)
{
	if (mobIndex >= 9860 && mobIndex <= 9871)// Zodiac
		return true;
	switch (mobIndex)
	{
		case 20371:// barones
		case 20367: // cata
		case 9023: // crystal
		case 20348:// deviltower
		case 30121:// beran
		case 9043:// eregon
		case 20394:// razador
		case 9045:// green lion
		case 20412: //jotun
		case 9042:// mushroom
		case 20419:// meley
		case 20173://orc maze
		case 9032:// sahmeran
		case 20395:// nemere
		case 9044:// trex
		case 9025:// vespik
		case 20423:// viserion
		case 9038: // caribbean
		case 9046:// fenrir
		case 90009:// hydra
			return true;
	}
	return false;
}
#endif