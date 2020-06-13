#include <string>
#include <vector>
#include <algorithm>

namespace util {

bool has_prefix(std::string st, std::string prefix) {
	return st.compare(0, prefix.size(), prefix) == 0;
}

std::string to_lower(std::string st) {
	std::transform(st.begin(), st.end(), st.begin(), [](unsigned char c){
		return std::tolower(c);
	});
	return st;
}

// TODO change to return indexes, rather than copying data? Faster. Does it matter?
std::vector<std::string> split(std::string st, std::string delim) {
	std::vector<std::string> strs;
	auto start = 0U;
	auto end = st.find(delim);
	while(end != std::string::npos)
	{
		strs.push_back(st.substr(start, end - start));
		start = end + delim.length();
		end = st.find(delim, start);
	}
	strs.push_back(st.substr(start, end));
	return strs;
}

std::string join(std::vector<std::string> st, std::string separator) {
	// TODO templatize, and use stringstream
	std::string str;
	for(size_t i = 0, end = st.size(); i != end; ++i) {
		str += st[i];
		if(i+1 != end) {
			str += separator;
		}
	}
	return str;
}

};
