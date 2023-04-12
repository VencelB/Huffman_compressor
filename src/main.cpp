#include "huffman.h"
#include <cstdio>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <cstring>
#include <fstream>



int main(int argc, char* argv[]) {
	int compress = -1;
	char* in_file = nullptr;
	char* out_file = nullptr;

	try {
		for (int i = 0; i < argc; i++) {
			if (strcmp(argv[i], "-c") == 0) {
				compress = 0;
			}
			if (strcmp(argv[i], "-u") == 0) {
				compress = 1;
			}
			if (strcmp(argv[i], "-f") == 0) {
				if (i == argc - 1) {
					throw CompressorExeption("Incorrect command form");
				}
				in_file = new char[strlen(argv[i + 1]) + 1];
				strcpy(in_file, argv[i + 1]);
			}
			if (strcmp(argv[i], "-o") == 0) {
				if (i == argc - 1) {
					throw CompressorExeption("Incorrect command form");
				}
				out_file = new char[strlen(argv[i + 1]) + 1];
				strcpy(out_file, argv[i + 1]);
			}
		}

		if (compress == -1 || in_file == nullptr || out_file == nullptr) {
			throw CompressorExeption("Incorrect command form");
		}
	}
	catch (CompressorExeption& exp) {
		if (in_file != nullptr) { delete[] in_file; }
		if (out_file != nullptr) { delete[] out_file; }
		return 0;
	}

	std::ifstream fin;
	try {
		fin.open(in_file, std::ios::in | std::ios::binary);
	}
	catch (...) {
		delete[] in_file;
		delete[] out_file;
		std::cout << "Unable to open file";
		return 0;
	}
	std::ofstream fout;
	try {
		fout.open(out_file, std::ios::out | std::ios::binary);
	}
	catch (...) {
		delete[] in_file;
		delete[] out_file;
		std::cout << "Unable to open file";
		fin.close();
		return 0;
	}

	delete[] in_file;
	delete[] out_file;

	if (compress == 0) {
		HaffmanArchive ha;
		std::string* st = new std::string();
		std::string* res;
		char buffer;

		while (true) {
			try {
				fin.read(&buffer, 1);
			}
			catch (...) {
				std::cout << "Reading error";
				delete st;
				return 0;
			}
			if (!fin) { break; }
			st->push_back(buffer);
		}
		std::cout << st->size() << std::endl;
		if (st->empty()) {
			res = new std::string();
		}
		else {
			res = ha.compress(st);
		}

		try {
			ha.write_coded_file(fout, res);
			delete st;
			delete res;
		}
		catch (CompressorExeption& exp) {
			ha.~HaffmanArchive();
			delete st;
			delete res;
			return 0;
		}
		catch (...) {
			ha.~HaffmanArchive();
			std::cout << "Writing error";
			delete st;
			delete res;
			return 0;
		}
	}
	else {
		HaffmanArchive ha;
		std::size_t d_size;
		std::string* st = nullptr;
		std::string* res = nullptr;
		try {
			st = ha.read_coded_file(fin, d_size);
			std::cout << (st->size() + 7) / 8 << std::endl;
			res = ha.decompress(st);
			std::cout << res->size() << "\n" << d_size << std::endl;
		}
		catch (CompressorExeption& exp) {
			if (st != nullptr) { delete st; }
			if (res != nullptr) { delete res; }
			return 0;
		}
		catch (...) {
			std::cout << "Writing error";
			delete st;
			delete res;
			return 0;
		}

		for (std::size_t i = 0; i < res->size(); i++) {
			try {
				fout.write(reinterpret_cast<const char*>(&(*res)[i]), 1);
			}
			catch (...) {
				std::cout << "Writing error";
				delete st;
				delete res;
				return 0;
			}
		}

		delete st;
		delete res;
	}

	fin.close();
	fout.close();
	return 0;
}