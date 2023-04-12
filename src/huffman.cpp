#include "huffman.h"
#include <cstdio>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <iostream>


HaffmanTreeNode::HaffmanTreeNode(HaffmanTreeNode* parent, HaffmanTreeNode* l_child, HaffmanTreeNode* r_child, const std::size_t& quantity, const std::string& value, const bool& is_list) {
	this->parent = parent;
	this->l_child = l_child;
	this->r_child = r_child;
	this->quantity = quantity;
	this->code = "";
	this->value = value;
	this->is_list = is_list;
}

HaffmanTreeNode* HaffmanTreeNode::get_parent() const { return this->parent; }
HaffmanTreeNode* HaffmanTreeNode::get_left_child() const { return this->l_child; }
HaffmanTreeNode* HaffmanTreeNode::get_right_child() const { return this->r_child; }
const std::string& HaffmanTreeNode::get_code() const { return this->code; }
const std::string& HaffmanTreeNode::get_value() const { return this->value; }
const std::size_t& HaffmanTreeNode::get_quantity() const { return this->quantity; }
const bool& HaffmanTreeNode::get_is_list() const { return this->is_list; }
void HaffmanTreeNode::increment() { this->quantity++; }

void HaffmanTreeNode::delete_child(HaffmanTreeNode* child) {
	if (child->parent->get_left_child() == child) {
		child->parent->l_child = nullptr;
	}
	if (child->parent->get_right_child() == child) {
		child->parent->r_child = nullptr;
	}
}

void HaffmanTreeNode::set_parent(HaffmanTreeNode* parent) { this->parent = parent; }
void HaffmanTreeNode::set_code(const std::string& code) { this->code = code; }

void HaffmanTree::add_list(const std::string& value) {
	bool flag = true;
	for (std::size_t i = 0; i < this->tree.size(); i++) {
		if (this->tree[i]->get_value() == value) {
			this->tree[i]->increment();
			flag = false;
		}
	}
	if (flag) {
		HaffmanTreeNode* new_list = new HaffmanTreeNode(nullptr, nullptr, nullptr, 1, value, true);
		if (new_list == nullptr) {
			throw CompressorExeption("Unable to allocate memory");
		}
		this->tree.push_back(new_list);
	}
}

void HaffmanTree::add_node(HaffmanTreeNode* left_child, HaffmanTreeNode* right_child) {
	HaffmanTreeNode* new_node = new HaffmanTreeNode(nullptr, left_child, right_child, (left_child->get_quantity() + right_child->get_quantity()), (left_child->get_value() + right_child->get_value()), false);
	if (new_node == nullptr) {
		throw CompressorExeption("Unable to allocate memory");
	}
	this->tree.push_back(new_node);
	left_child->set_parent(new_node);
	right_child->set_parent(new_node);
	this->tree.erase(std::find(this->tree.begin(), this->tree.end(), left_child));
	this->tree.erase(std::find(this->tree.begin(), this->tree.end(), right_child));
}

void HaffmanTree::erase(HaffmanTreeNode* node) {
	if (node->get_left_child() != nullptr) {
		erase(node->get_left_child());
	}
	if (node->get_right_child() != nullptr) {
		erase(node->get_right_child());
	}
	if (node->get_parent() != nullptr) {
		node->get_parent()->delete_child(node);
	}
	delete node;
}

void HaffmanTree::sort() {
	HaffmanTreeNode* tmp;
	for (std::size_t i = 0; i < this->tree.size() - 1; i++) {
		for (std::size_t j = 0; j < this->tree.size() - 1; j++) {
			if (this->tree[j]->get_quantity() < this->tree[j + 1]->get_quantity()) {
				tmp = this->tree[j];
				this->tree[j] = this->tree[j + 1];
				this->tree[j + 1] = tmp;
			}
		}
	}
}

void HaffmanTree::make_tree() {
	if (this->tree.size() == 1) {
		HaffmanTreeNode* new_node = new HaffmanTreeNode(nullptr, this->tree[0], nullptr, this->tree[0]->get_quantity(), this->tree[0]->get_value(), false);
		if (new_node == nullptr) {
			throw CompressorExeption("Unable to allocate memory");
		}
		this->tree[0]->set_parent(new_node);
		this->root = new_node;
	}
	else {
		while (this->tree.size() > 1) {
			this->sort();
			this->add_node(this->tree[this->tree.size() - 1], this->tree[this->tree.size() - 2]);
		}
		this->root = this->tree[0];
	}
}

void HaffmanTree::make_code(HaffmanTreeNode* node) {
	if (node->get_left_child() != nullptr) {
		node->get_left_child()->set_code(node->get_code() + "0");
		this->make_code(node->get_left_child());
	}
	if (node->get_right_child() != nullptr) {
		node->get_right_child()->set_code(node->get_code() + "1");
		this->make_code(node->get_right_child());
	}
}

void HaffmanTree::make_dict(HaffmanTreeNode* node) {
	if (node->get_is_list()) {
		this->dict[node->get_value()] = node->get_code();
	}
	if (node->get_left_child() != nullptr) {
		this->make_dict(node->get_left_child());
	}
	if (node->get_right_child() != nullptr) {
		this->make_dict(node->get_right_child());
	}
}

HaffmanTreeNode* HaffmanTree::get_root() const { return this->root; }
std::string HaffmanTree::get_value_from_dict(std::string key) { return this->dict[key]; }

std::size_t HaffmanTree::write_dict(std::ofstream& fout) const {
	std::size_t wrote_size = 0;
	std::size_t dict_size = this->dict.size();
	std::size_t value_size;
	fout.write(reinterpret_cast<const char*>(&dict_size), sizeof(std::size_t));
	wrote_size += sizeof(std::size_t);
	for (const auto& [key, value] : this->dict) {
		char ckey = key[0];
		fout.write(&ckey, 1);
		value_size = value.size();
		fout.write(reinterpret_cast<const char*>(&value_size), sizeof(std::size_t));
		wrote_size += (1 + sizeof(std::size_t));
		for (std::size_t i = 0; i < value.size(); i++) {
			fout.write(reinterpret_cast<const char*>(&value[i]), 1);
			wrote_size += 1;
		}
	}
	return wrote_size;
}

void HaffmanArchive::make_code_dict(std::string* st) {
	std::string strsmb;
	for (std::size_t i = 0; i < st->size(); i++) {
		strsmb = "";
		strsmb.push_back((*st)[i]);
		this->ht.add_list(strsmb);
	}

	this->ht.make_tree();
	this->ht.make_code(this->ht.get_root());
	this->ht.make_dict(this->ht.get_root());
	this->ht.erase(this->ht.get_root());
}

std::string* HaffmanArchive::compress(std::string* st) {
	this->make_code_dict(st);
	std::string* res = new std::string();
	std::string strsmb;
	for (std::size_t i = 0; i < st->size(); i++) {
		strsmb = "";
		strsmb.push_back((*st)[i]);
		(*res) += this->ht.get_value_from_dict(strsmb);
	}
	return res;
}

void HaffmanArchive::write_coded_file(std::ofstream& fout, std::string* coded_st) {
	std::size_t d_size = this->ht.write_dict(fout);
	std::size_t code_size = 0;
	std::size_t str_size = coded_st->size();
	fout.write(reinterpret_cast<const char*>(&str_size), sizeof(std::size_t));
	d_size += sizeof(std::size_t);
	while (coded_st->size() % 8 != 0) {
		coded_st->push_back('0');
	}
	unsigned char buffer;
	for (std::size_t i = 0; i < coded_st->size(); i += 8) {
		buffer = 0;
		for (std::size_t j = 0; j < 8; j++) {
			if ((*coded_st)[i + j] == '0') {
				buffer = buffer * 2;
			}
			if ((*coded_st)[i + j] == '1') {
				buffer = buffer * 2 + 1;
			}
		}
		fout.write(reinterpret_cast<const char*>(&buffer), 1);
		code_size++;
	}
	std::cout << code_size << "\n" << d_size << std::endl;
}

std::string* HaffmanArchive::read_coded_file(std::ifstream& fin, std::size_t& d_size) {
	std::size_t dict_size, sizeof_value, code_len;
	char key;
	unsigned char elem;
	std::string value;
	d_size = 0;
	fin.read(reinterpret_cast<char*>(&dict_size), sizeof(std::size_t));
	d_size += sizeof(std::size_t);
	std::string strkey;
	for (std::size_t i = 0; i < dict_size; i++) {
		fin.read(&key, 1);
		fin.read(reinterpret_cast<char*>(&sizeof_value), sizeof(std::size_t));
		value = "";
		d_size += sizeof(std::size_t) + 1;
		for (std::size_t j = 0; j < sizeof_value; j++) {
			fin.read(reinterpret_cast<char*>(&elem), 1);
			value.push_back(elem);
			d_size += 1;
		}
		strkey = "";
		strkey.push_back(key);
		this->decode_dict[value] = strkey;
	}
	fin.read(reinterpret_cast<char*>(&code_len), sizeof(std::size_t));
	d_size += sizeof(std::size_t);
	std::string buffer;
	std::string* res = new std::string();
	if (res == nullptr) {
		throw CompressorExeption("Unable to allocate memory");
	}
	for (std::size_t i = 0; i < (code_len + 7) / 8; i++) {
		buffer = "";
		fin.read(reinterpret_cast<char*>(&elem), 1);
		for (std::size_t j = 0; j < 8; j++) {
			if (elem % 2 == 0) {
				buffer.push_back('0');
			}
			else {
				buffer.push_back('1');
			}
			elem = elem / 2;
		}
		std::reverse(buffer.begin(), buffer.end());
		if (i == ((code_len + 7) / 8) - 1) {
			std::size_t t = ((code_len + 7) / 8) * 8 - code_len;
			while (t > 0) {
				buffer.pop_back();
				t--;
			}
		}
		(*res) += buffer;
	}
	return res;
}

std::string* HaffmanArchive::decompress(std::string* st) {
	std::string cur_code = "";
	std::string* res = new std::string();
	if (res == nullptr) {
		throw CompressorExeption("Unable to allocate memory");
	}
	for (std::size_t i = 0; i != st->size(); i++) {
		cur_code += (*st)[i];
		if (this->decode_dict.count(cur_code)) {
			(*res) += this->decode_dict[cur_code];
			cur_code = "";
		}
	}
	return res;
}
