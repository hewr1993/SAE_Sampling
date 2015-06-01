#include "table_generator.h"
#include <assert.h>
using namespace std;

void TableGenerator::setTitle(vector<string> title) {
	this->title = title;
}

void TableGenerator::addRow(vector<string> row, int line) {
	if (line < 0 || line >= row.size()) {
		table.push_back(row);
	} else {
		table.insert(table.begin() + line, row);
	}
}

void TableGenerator::delRow(int line) {
	assert(table.size() > 0);
	if (line < 0 || line >= table.size()) 
		line = table.size() - 1;
	table.erase(table.begin() + line);
}

// only support markdown now
string TableGenerator::report(string format) {
	string newline = "\n";
	// calculate widths
	int col = title.size();
	for (int i = 0; i < table.size(); ++i) col = max(col, (int) table[i].size());
	vector<int> margin(col, 2);
	for (int i = 0; i < title.size(); ++i) margin[i] = max(margin[i], (int) title[i].size() + 2);
	for (int i = 0; i < table.size(); ++i) for (int j = 0; j < table[i].size(); ++j) 
		margin[j] = max(margin[j], (int) table[i][j].size() + 2);
	string ret;
	// title
	{
		if (format == "markdown") ret = "|";
		for (int i = 0; i < col; ++i) {
			string content;
			if (i < title.size()) {
				content = title[i];
			} else {
				//content = 'A' + ((i - title.size()) % 26);
			}
			int left = (margin[i] - content.length()) / 2; // whitespaces
			int right = margin[i] - content.length() - left;
			ret += string(left, ' ') + content + string(right, ' ');
			if (format == "markdown") ret += "|";
		}
		ret += newline;
		if (format == "markdown") {
			ret += "|";
			for (int i = 0; i < margin.size(); ++i) ret += string(margin[i], '-') + "|";
		}
		ret += newline;
	}
	// content
	for (int i = 0; i < table.size(); ++i) {
		ret += "|";
		for (int j = 0; j < col; ++j) {
			string content;
			if (j < table[i].size()) content = table[i][j];
			int left = (margin[j] - content.length()) / 2; // whitespaces
			int right = margin[j] - content.length() - left;
			ret += string(left, ' ') + content + string(right, ' ');
			if (format == "markdown") ret += "|";
		}
		ret += newline;
	}
	return ret;
}
