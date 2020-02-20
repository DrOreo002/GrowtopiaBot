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

static string getCommand() {
	string command;
	cin >> command;
	return command;
}

void execCommand() {
	while (true) {
		string cmd = getCommand(); 

		// Make custom command!
	}
}

inline bool exists_test(const string& name) {
	ifstream f(name.c_str());
	return f.good();
}

int main() {
	init();
	cout << "GrowtopiaBot v1.0 [Not included QT]! by GrowtopiaNoobs and DrOreo002" << endl;

	string botDataPath = "bot.json";
	if (!exists_test(botDataPath)) {
		cout << "Please specify your bot data path... [Json]" << endl;
		cin >> botDataPath;
		return 0;
	}
	cout << "Loading bot data..." << botDataPath << endl;

	ifstream i(botDataPath);
	json j;
	i >> j;

	cout << j["username"] << endl;

	GrowtopiaBot bot = {
		j["username"], j["password"]
	};
	cout << "Bot loaded!. Using username " << j["username"] << endl;

	bot.gameVersion = j["gameVersion"].get<string>();
	bot.worldName = j["targetWorld"].get<string>();
	bot.publicOwnership = j["publicOwnership"].get<bool>();
	if (!bot.publicOwnership) {
		// TODO : Make owner...
		//bot.owner = j["botOwner"].get<string>();
	}

	bot.userInit();
	bots.push_back(bot);
	thread t1(execCommand);

	while (true) {
		bot.eventLoop();
	}
}
