#include <cstddef>
#include <string>
#include <ncurses.h>
#include <vector>

#include "EggLog.hpp"

namespace EggLog {
	std::vector<std::string> logs;

	void Push(const std::string str) {
		logs.push_back(str);
	}

	void Clear() {
		logs.clear();
	}

	bool Empty() {
		return logs.empty();
	}

	std::size_t Count() {
		return logs.size();
	}

	void Render() {
		if(Empty()) return;
		const int ymax = getmaxy(stdscr);
		const int xmax = getmaxx(stdscr);
		wmove(stdscr, ymax - Count() - 2, 1);
		hline(ACS_HLINE, getmaxx(stdscr) - Count() + 1);
		mvaddch(ymax - Count() - 2, xmax - 1, ACS_RTEE);
		mvaddch(ymax - Count() - 2, 0, ACS_LTEE);
		std::size_t lineCount = Count();
		for(std::string& str : logs) {
			mvprintw(ymax - lineCount - 1, 2, "%s", str.c_str());
			lineCount--;
		}
	}
};
