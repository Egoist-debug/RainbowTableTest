#pragma once
#include <vector>
#include <map>
#include <set>
#include <string>
class RainbowTable
{
public:
	RainbowTable() = default;
	RainbowTable(size_t size, int chainLength, int passwordLength, std::string hash);
	~RainbowTable() = default;
	void CreateTable();
	void SaveTable();
	std::string Hash(std::string password);
	std::string Reduce(std::string hash, int salt);
	static int fileNum;
	void SetHash(std::string hash);
	void SetPasswordLength(int passwordLength);
private:
	size_t mSize;
	int mChainLength;
	int mPasswordLength;
	const std::string chars = "abcdefghijklmnopqrstuvwxyz0123456789";
	std::string mHash;
	std::map<std::string, std::string> mTable;
	std::set<std::string> mPasswords;
	void GenerateRandomPasswords();
	std::string CreateChain(std::string password);

};


class RainbowTableCrack
{
public:
	RainbowTableCrack() = default;
	RainbowTableCrack(std::string hash, std::string filename);
	~RainbowTableCrack() = default;
	void Crack();
	void LoadTable();
private:
	int mChainLength;
	int mPasswordLength;
	std::string mHash;
	std::string mHashType;
	std::string mTableFileName;
	RainbowTable mRainbowTable;
	std::map<std::string, std::string> mTable;
	std::string Find(std::string str);
	void Crack(std::string str, int index, std::vector<std::pair<int, std::string>>& result);
};