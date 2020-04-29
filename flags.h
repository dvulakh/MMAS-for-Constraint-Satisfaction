
#ifndef FLAGS_H
#define FLAGS_H

#include <unordered_map>
#include <functional>
#include <vector>
#include <string>
#include <list>

using namespace std;

#define FLAGS_INPUT_SUCCESS 0
#define FLAGS_INVALID_INPUT 1

typedef pair<int, function<void(vector<string>)>> flag_key;

// Structure for reading command-line-flag input
struct flag_reader
{
	unordered_map<char, string> full_names;
	unordered_map<string, flag_key> exec;
	flag_reader() {}
	/// Add flag definition
	void add_flag(string name, char nick, int narg, function<void(vector<string>)> eff);
	/// Read input with flags given
	int read_flags(int narg, char** args);
};

#endif
