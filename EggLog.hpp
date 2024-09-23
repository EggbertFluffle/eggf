#pragma once

#include <cstddef>
#include <string>
#include <ncurses.h>

namespace EggLog {
	void Push(const std::string str);
	void Pop();
	void Clear();
	std::size_t Count();
	bool Empty();
	void Render();
};
