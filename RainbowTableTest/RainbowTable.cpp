#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <memory>
#include <random>
#include <openssl/evp.h>
#include "RainbowTable.h"


RainbowTable::RainbowTable(size_t size, int chainLength, int passwordLength, std::string hash)
	: mSize(size), mChainLength(chainLength), mPasswordLength(passwordLength), mHash(hash)
{
}

// 使用EVP接口计算hash值
std::string RainbowTable::Hash(std::string password)
{
	const EVP_MD* engine = EVP_get_digestbyname(mHash.c_str());
	if (engine == nullptr)
	{
		std::cout << "no such digest" << std::endl;
		return "";
	}
	EVP_MD_CTX* ctx = EVP_MD_CTX_new();
	EVP_DigestInit_ex(ctx, engine, nullptr);
	EVP_DigestUpdate(ctx, password.c_str(), password.length());
	unsigned char md[EVP_MAX_MD_SIZE] = { 0 };
	unsigned int length = 0;
	EVP_DigestFinal_ex(ctx, md, &length);
	EVP_MD_CTX_free(ctx);
	char buf[3] = { 0 };
	std::string hash = "";
	for (unsigned int i = 0; i < length; i++)
	{
		sprintf(buf, "%02x", md[i]);
		hash += buf;
	}
	// 输出hash值
	//std::cout << hash << std::endl;
	return hash;
}

// 规约函数，将hash值缩减为密码，保证包含大小写字母和数字
std::string RainbowTable::Reduce(std::string hash, int salt)
{
	std::string password = "";
	int index = 0;
	for (int i = 0; i < mPasswordLength; i++)
	{
		index = (int)hash[i] + salt;
		password += chars[index % chars.length()];
	}
	return password;
}

void RainbowTable::SetHash(std::string hash)
{
	mHash = hash;
}

void RainbowTable::SetPasswordLength(int passwordLength)
{
	mPasswordLength = passwordLength;
}

void RainbowTable::GenerateRandomPasswords()
{
	// 生成随机密码，包含大小写字母和数字,密码长度为mPasswordLength
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, chars.length() - 1);
	std::string password = "";
	for (int i = 0; i < mSize; i++)
	{
		password = "";
		for (int j = 0; j < mPasswordLength; j++)
		{
			password += chars[dis(gen)];
		}
		mPasswords.insert(password);
	}
	
}

std::string RainbowTable::CreateChain(std::string password)
{
	std::string hash = "";
	std::string reduced = password;
	for (int i = 0; i < mChainLength; i++)
	{
		hash = Hash(reduced);
		reduced = Reduce(hash, i);
	}
	return reduced;
}

void RainbowTable::CreateTable()
{
	GenerateRandomPasswords();
	std::string end = "";
	std::string password = "";
	std::set<std::string>::iterator it;
	for (it = mPasswords.begin(); it != mPasswords.end(); it++)
	{
		password = *it;
		end = CreateChain(password);
		mTable.insert(std::pair<std::string, std::string>(password, end));
	}
}

// 将生成的彩虹表保存到文件中，如果文件已存在则覆盖
void RainbowTable::SaveTable()
{
	std::string filename = "table_"+ std::to_string(mSize) + "_" + mHash + "_"+ std::to_string(mPasswordLength) + "_" + std::to_string(mChainLength) + ".txt";
	std::ofstream file;
	file.open(filename);
	std::map<std::string, std::string>::iterator it;
	for (it = mTable.begin(); it != mTable.end(); it++)
	{
		file << it->first << " " << it->second << std::endl;
	}
	file.close();
	// 将内存中的彩虹表清空
	mTable.clear();
	mPasswords.clear();
	
}

// 从文件中读取彩虹表
void RainbowTableCrack::LoadTable()
{
	// 将文件名按照_分割，获取彩虹表的参数
	std::string filename = mTableFileName;
	std::string delimiter = "_";
	std::string token;
	size_t pos = 0;
	std::vector<std::string> params;
	while ((pos = filename.find(delimiter)) != std::string::npos) {
		token = filename.substr(0, pos);
		params.push_back(token);
		filename.erase(0, pos + delimiter.length());
	}
	params.push_back(filename);
	mHashType = params[2];
	mPasswordLength = std::stoi(params[3]);
	mChainLength = std::stoi(params[4]);
	std::ifstream file;
	file.open(mTableFileName);
	std::string password = "";
	std::string end = "";
	while (file >> password >> end)
	{
		mTable.insert(std::pair<std::string, std::string>(password, end));
	}
	file.close();
}

// 根据hash值查找对应的密码
void RainbowTableCrack::Crack()
{
	std::string hash = mHash;
	std::vector<std::pair<int, std::string>> result;
	Crack(hash, mChainLength - 1, result);
	if (result.size() != 0)
	{
		std::vector<std::pair<int, std::string>>::iterator it;
		for (it = result.begin(); it != result.end(); it++)
		{
			std::string hash = "";
			std::string reduced = "";
			reduced = it->second;
			hash = mRainbowTable.Hash(reduced);
			for (int i = 0; i < it->first; i++)
			{
				reduced = mRainbowTable.Reduce(hash, i);
				hash = mRainbowTable.Hash(reduced);
			}
			if (hash == mHash)
			{
				std::cout << "Found password: " << reduced << std::endl;
				return;
			}
		}
		std::cout << "Not found password!" << std::endl;
		return;
	}
	else
	{
		std::cout << "Not found password!" << std::endl;
		return;
	}
}

RainbowTableCrack::RainbowTableCrack(std::string hash, std::string filename)
	: mHash(hash),mTableFileName(filename)
{
	LoadTable();
	mRainbowTable.SetHash(mHashType);
	mRainbowTable.SetPasswordLength(mPasswordLength);
}

// 找到彩虹表中对应的链，返回密码和索引
void RainbowTableCrack::Crack(std::string hash, int index, std::vector<std::pair<int, std::string>>& result)
{
	if (index == -1)
	{
		return;
	}
	std::vector<std::string> passwords;
	std::string reduced = "";
	std::string hash1 = "";
	hash1 = hash;
	for (int i = index; i < mChainLength; i++)
	{
		reduced = mRainbowTable.Reduce(hash1, i);
		hash1 = mRainbowTable.Hash(reduced);
	}
	Find(reduced, passwords);
	if (passwords.size() != 0)
	{
		std::vector<std::string>::iterator it;
		for (it = passwords.begin(); it != passwords.end(); it++)
		{
			result.push_back(std::pair<int, std::string>(index, *it));
		}
	}
	Crack(hash, index - 1, result);
}

void RainbowTableCrack::Find(std::string str, std::vector<std::string>& passwords)
{
	std::map<std::string, std::string>::iterator it;
	for (it = mTable.begin(); it != mTable.end(); it++)
	{
		if (it->second == str)
		{
			passwords.push_back(it->first);
		}
	}
	return;
}