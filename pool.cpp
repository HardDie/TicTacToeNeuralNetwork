#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include "pool.h"

/********************
 *     Static
 ********************/

static std::random_device random_device;
static std::mt19937 engine(random_device());

static int rotateValue90Degree(int value) {
	switch(value) {
	case 0: return 6;
	case 1: return 3;
	case 2: return 0;
	case 3: return 7;
	case 4: return 4;
	case 5: return 1;
	case 6: return 8;
	case 7: return 5;
	case 8: return 2;
	default:
		std::cerr << __FUNCTION__
		          << "(): Wrong value for rotation!\n";
		exit(-1);
	}
}

static int rotateValue180Degree(int value) {
	switch(value) {
	case 0: return 8;
	case 1: return 7;
	case 2: return 6;
	case 3: return 5;
	case 4: return 4;
	case 5: return 3;
	case 6: return 2;
	case 7: return 1;
	case 8: return 0;
	default:
		std::cerr << __FUNCTION__
		          << "(): Wrong value for rotation!\n";
		exit(-1);
	}
}

static int rotateValue270Degree(int value) {
	switch(value) {
	case 0: return 2;
	case 1: return 5;
	case 2: return 8;
	case 3: return 1;
	case 4: return 4;
	case 5: return 7;
	case 6: return 0;
	case 7: return 3;
	case 8: return 6;
	default:
		std::cerr << __FUNCTION__
		          << "(): Wrong value for rotation!\n";
		exit(-1);
	}
}

/********************
 *     Public
 ********************/

Pool::Pool(void) :
	returnedValue_(-1)
{
}

Pool::Pool(const std::string& string) :
	returnedValue_(-1)
{
	std::string s = string;
	std::string delimiter = " ";
	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter))
	       != std::string::npos) {
		token = s.substr(0, pos);
		this->AddStep(std::stoi(token));
		s.erase(0,
		    pos + delimiter.length());
	}
}

Pool::Pool(const std::vector<int> &pool) :
	returnedValue_(-1),
	stepsPool_(pool)
{
}

Pool::~Pool(void) {
	if (returnedValue_ != -1) {
		std::cerr << __FUNCTION__
		          << "(): Remove pool without save progress!\n";
	}
}

bool Pool::AddStep(int value) {
	if (value < 0 || value > 8) {
		std::cerr << __FUNCTION__
		          << "(): Invalid step!\n";
		return false;
	}

	stepsMap_[value]++;
	return true;
}

bool Pool::WasGoodStep(void) {
	if (returnedValue_ == -1) {
		std::cerr << __FUNCTION__
		          << "(): Try save empty value!\n";
		return false;
	}

	this->AddStep(returnedValue_);
	returnedValue_ = -1;
	return true;
}

bool Pool::WasBadStep(void) {
	if (returnedValue_ == -1) {
		std::cerr << __FUNCTION__
		          << "(): Try save empty value!\n";
		return false;
	}

	returnedValue_ = -1;
	return true;
}

int Pool::GetStep(int degree) {
	int countObjects = 0;
	for_each(stepsMap_.begin(), stepsMap_.end(),
	         [&countObjects](std::pair<int, int> value) {
		         countObjects += value.second;
	         });

	if (!countObjects) {
		std::cerr << __FUNCTION__
		          << "(): No available steps in pool!\n";
		exit(-1);
	}

	std::uniform_int_distribution<int> dist(0, countObjects - 1);
	int randValue = dist(engine);

	int tmpRange = 0;
	for (auto value: stepsMap_) {
		// Search value at correct range
		if (randValue >= tmpRange &&
		    randValue < tmpRange + value.second) {
			returnedValue_ = value.first;
			break;
		}
		tmpRange += value.second;
	}

	switch(degree) {
	case 0: return returnedValue_;
	case 90: return rotateValue90Degree(returnedValue_);
	case 180: return rotateValue180Degree(returnedValue_);
	case 270: return rotateValue270Degree(returnedValue_);
	default:
		std::cerr << __FUNCTION__
		          << "(): Wrong degree!\n";
		exit(-1);
	}
}

void Pool::Print(void) const{
	for (auto it: stepsMap_) {
		std::cout << it.first << "(" << it.second << ") ";
	}

	std::cout << "last(" << returnedValue_ << ")" << std::endl;
}

bool Pool::SaveToFile(std::string fileName) const {
	std::ofstream file;
	file.open(fileName, std::ios::app);
	if (!file.is_open()) {
		std::cerr << __FUNCTION__
		          << "(): Can't open file for save pool\n";
		return false;
	}
	for (int value: stepsPool_) {
		file << value << " ";
	}
	file << std::endl;
	file.close();
	return true;
}

bool Pool::SaveToBinFile(std::ofstream& file) const {
	std::size_t len = stepsPool_.size();
	file.write((char*)&len, sizeof(len));
	for (int el: stepsPool_) {
		file.write((char*)&el, sizeof(el));
	}
	return true;
}

bool Pool::LoadFromBinFile(std::ifstream& file) {
	std::size_t len;
	int newElement;
	file.read((char*)&len, sizeof(len));
	for (std::size_t i = 0; i < len; i++) {
		file.read((char*)&newElement, sizeof(newElement));
		stepsPool_.push_back(newElement);
	}
	return true;
}

/********************
 *     Private
 ********************/

int Pool::GetMapValue(int index) const {
	try {
		return stepsMap_.at(index);
	} catch(...) {
		// If element not exist, just return 0;
		return 0;
	}
}
