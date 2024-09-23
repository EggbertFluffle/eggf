#pragma once

#include <filesystem>

struct DirectoryEntry {
	const uint position;
	const std::filesystem::directory_entry entry;

	DirectoryEntry(const std::filesystem::directory_entry& _entry, const uint _pos);

	void Render(const std::size_t& currentLine) const;
};
