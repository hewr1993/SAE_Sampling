#pragma once

#include <vector>
#include <sstream>
#include <iostream>

template<typename A> std::string toString(A a) {
	std::stringstream sin;
	sin << a;
	return sin.str();
}

class TableGenerator {
public:
	TableGenerator() {}
	~TableGenerator() {}
	void setTitle(std::vector<std::string>);
	void addRow(std::vector<std::string>, int line=-1); // default: add to tail
	void delRow(int line=-1); // default: delete the last
	std::string report(std::string format="markdown"); // return table content in format of Markdown
private:
	std::vector<std::string> title;
	std::vector<std::vector<std::string> > table;
};
