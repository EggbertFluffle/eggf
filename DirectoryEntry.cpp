#include <filesystem>
#include <ncurses.h>

#include "DirectoryEntry.hpp"
#include "App.hpp"

DirectoryEntry::DirectoryEntry(const std::filesystem::directory_entry& _entry, const uint _pos): position(_pos), entry(_entry) {}

void DirectoryEntry::Render(const std::size_t& currentLine) const {
	const int colorPair = (entry.is_directory() ? PAIR_DIRECTORY : PAIR_REGULAR) + ((currentLine == position) ? 1 : 0);
	attron(COLOR_PAIR(colorPair));
	mvprintw(position + 1, 2, "%s", entry.path().filename().c_str());
	attroff(COLOR_PAIR(colorPair));
}
