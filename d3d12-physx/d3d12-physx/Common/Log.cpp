#pragma once

#include <iostream>
#include <fstream>
#include <ctime>

#include "Log.h"

// 基于当前系统的当前日期/时间
time_t now = time(0);

// 返回一个指向表示本地时间的 tm 结构的指针
tm* ltm = localtime(&now);

std::string name =  std::to_string(1900 + ltm->tm_year) + '-' +
					std::to_string(1 + ltm->tm_mon) + '-' +
					std::to_string(ltm->tm_mday) + '-' +
					std::to_string(ltm->tm_hour) + '-' +
					std::to_string(ltm->tm_min) + '-' +
					std::to_string(ltm->tm_sec) + ".txt";

std::ofstream out("Log/" + name);

void Log(std::string str) {

	std::cout << str << std::endl;

	out << str << std::endl;
}