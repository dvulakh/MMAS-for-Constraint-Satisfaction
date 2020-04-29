
#include "flags.h"

// Add new flag to nickname map and effects map
void flag_reader::add_flag(string name, char nick, int narg, function<void(vector<string>)> eff)
{
	exec.insert({ name, { narg, eff } });
	if (nick)
		full_names.insert({ nick, name });
}

// Read input string
int flag_reader::read_flags(int narg, char** args)
{
	list<string> flag_queue;
	int sn = 1;
	while (sn < narg) {
		string s = string(args[sn]);
		/// New flag(s)
		if (s[0] == '-') {
			/// No flags
			if (s.length() < 2)
				return FLAGS_INVALID_INPUT;
			/// Long flag
			else if (s[1] == '-')
				flag_queue.push_back(s.substr(2, s.length()));
			/// Small flags
			else
				for (int i = 1; i < s.length(); i++) {
					auto it = full_names.find(s[i]);
					if (it == full_names.end())
						return FLAGS_INVALID_INPUT;
					flag_queue.push_back(it->second);
				}
			sn++;
		}
		/// New argument(s)
		else {
			/// No flags waiting
			if (flag_queue.empty())
				return FLAGS_INVALID_INPUT;
			/// Execute function
			string f = flag_queue.front(); flag_queue.pop_front();
			auto it = exec.find(f);
			if (it == exec.end())
				return FLAGS_INVALID_INPUT;
			flag_key fk = it->second;
			vector<string> ar;
			for (int i = 0; i < fk.first; i++)
				if (sn + i >= narg)
					return FLAGS_INVALID_INPUT;
				else
					ar.push_back(string(args[sn + i]));
			try { fk.second(ar); }
			catch (exception e) { return FLAGS_INVALID_INPUT; }
			sn += fk.first;
		}
	}
	/// Leftover flags must take no arguments
	for (string s : flag_queue) {
		auto it = exec.find(s);
		if (it == exec.end() || it->second.first)
			return FLAGS_INVALID_INPUT;
		try { it->second.second(vector<string>()); }
		catch (exception e) { return FLAGS_INVALID_INPUT; }
	}
	return FLAGS_INPUT_SUCCESS;
}
