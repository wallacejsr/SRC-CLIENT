#include "stdafx.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::string FromHex(const std::string& in) {
	std::string output;
	if ((in.length() % 2) != 0) {
		std::ofstream errorFile("HackShield.Config.Error.txt");
		errorFile << "[c0d3xShield] Error while decrypting this text: " << in << "\n";
		errorFile.close();
		throw std::runtime_error("Error while decrypting");
	}
	
	size_t cnt = in.length() / 2;
	for (size_t i = 0; cnt > i; ++i) {
		unsigned long int s = 0;
		std::stringstream ss;
		ss << std::hex << in.substr(i * 2, 2);
		ss >> s;
		output.push_back(static_cast<unsigned char>(s));
	}

	return output;
}

std::string UltimateCipher(std::string Str) {
	char Key[3] = { 'A', 'B', 'C' };
	std::string Encrypted = Str;
	for (unsigned int i = 0; i < Str.size(); i++) Encrypted[i] = Str[i] ^ Key[i % (sizeof(Key) / sizeof(char))];
	return Encrypted;
}

std::string UltimateDecrypt(std::string MyString) {
	return UltimateCipher(FromHex(MyString));
}

int main() {
	// Exemplo de uso
	const char Key[3] = { 'A', 'B', 'C' };
	std::string originalText = "Hello, World!";
	std::string encryptedText = UltimateCipher(originalText);
	std::string decryptedText = UltimateDecrypt(encryptedText);
	
	// Adicione mais lógica conforme necessário...
	
	return 0;
}
