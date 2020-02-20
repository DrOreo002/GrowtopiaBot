// CPPBot.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include "pch.h"
#include <iostream>
#include <fstream>
#include <future>
#include <thread>
#include <chrono>
#include "corefunc.h"
#include "userfunc.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

vector<GrowtopiaBot> bots;

inline bool exists_test(const string& name) {
	ifstream f(name.c_str());
	return f.good();
}

int main() {
	init();
	cout << "GrowtopiaBot v1.0 [Not included QT]! by GrowtopiaNoobs and DrOreo002" << endl;

	string botDataPath = "H:\\anom\\CPP Project\\GrowtopiaBot\\bot.json";
	if (!exists_test(botDataPath)) {
		cout << "Please specify your bot data path... [Json]" << endl;
		cin >> botDataPath;
		return 0;
	}
	cout << "Loading bot data on path (" << botDataPath << ")" << endl;

	ifstream i(botDataPath);
	json j;
	i >> j;

	GrowtopiaBot bot = {
		j["username"], j["password"]
	};
	cout << "------------------------" << endl;
	cout << "Bot loaded!. Using username " << j["username"] << endl;

	bot.gameVersion = j["gameVersion"].get<string>();
	bot.worldName = j["targetWorld"].get<string>();

	bot.userInit();
	bots.push_back(bot);

	while (true) {
		bot.eventLoop();
	}
}
