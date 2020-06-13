#include <cstdio>

#include "repl.hpp"

#include "util.hpp"
#include <string>
#include <cstdlib>
#include <ctime>

int main() {
	std::srand(std::time(nullptr)); // TODO use object, make deterministic.
	return repl();
}
