#pragma once

#include <iostream>
#include <fstream>
#include <ctime>

#include "Log.h"

// ���ڵ�ǰϵͳ�ĵ�ǰ����/ʱ��
time_t now = time(0);

// ����һ��ָ���ʾ����ʱ��� tm �ṹ��ָ��
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