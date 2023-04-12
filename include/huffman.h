#ifndef HAFFMAN_H
#define HAFFMAN_H

#include <stddef.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>

class HaffmanTreeNode {
public:
	HaffmanTreeNode(HaffmanTreeNode* parent, HaffmanTreeNode* l_child, HaffmanTreeNode* r_child, const std::size_t& quantity, const std::string& value, const bool& is_list);
	HaffmanTreeNode* get_parent() const;
	HaffmanTreeNode* get_left_child() const;
	HaffmanTreeNode* get_right_child() const;
	const std::string& get_code() const;
	const std::string& get_value() const;
	const std::size_t& get_quantity() const;
	const bool& get_is_list() const;
	void increment();
	void delete_child(HaffmanTreeNode* child);
	void set_parent(HaffmanTreeNode* parent);
	void set_code(const std::string& code);

private:
	HaffmanTreeNode* parent;
	HaffmanTreeNode* l_child;
	HaffmanTreeNode* r_child;
	std::size_t quantity;
	std::string code;
	std::string value;
	bool is_list;
};


class HaffmanTree {
public:
	void add_list(const std::string& value);
	void add_node(HaffmanTreeNode* left_child, HaffmanTreeNode* right_child);
	void erase(HaffmanTreeNode* node);
	void sort();
	void make_tree();
	void make_code(HaffmanTreeNode* node);
	void make_dict(HaffmanTreeNode* node);
	HaffmanTreeNode* get_root() const;
	std::string get_value_from_dict(std::string key);
	std::size_t write_dict(std::ofstream& fout) const;

private:
	HaffmanTreeNode* root;
	std::vector<HaffmanTreeNode*> tree;
	std::map<std::string, std::string> dict;
};


class HaffmanArchive {
public:
	void make_code_dict(std::string* st);
	std::string* compress(std::string* st);
	void write_coded_file(std::ofstream& fout, std::string* coded_st);
	std::string* read_coded_file(std::ifstream& fin, std::size_t& d_size);
	std::string* decompress(std::string*);
private:
	HaffmanTree ht;
	std::map<std::string, std::string> decode_dict;
};

class CompressorExeption : public std::exception {
public:
	CompressorExeption(const std::string& error) {
		std::cout << error << std::endl;
	}
};

#endif