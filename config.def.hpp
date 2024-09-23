#pragma once

#include <string>
#include <vector>

#ifndef USER_CONFIG
namespace USER_CONFIG{
	// Preffered file handlers where %s is the file path
	const std::vector<std::pair<std::vector<std::string>, std::string>> FILE_HANDLERS = {
	  // File extensions   |    Command
		{ {"png", "jpg"},      "feh %s" },
		{ {"mp3", "mp4"},      "mpv %s" },
		{ {"html"},         "firefox %s"}
	};
	const std::string EDITOR = "vi %s";

	// Preview text files on the right side of the screen
	const bool FILE_PREVIEW = true;

	// Measured as a percent of the screen width
	const float PREVIEW_SIZE = 0.5;

	// What to replace tabs with in the file preview
	const std::string TAB_REPLACE = "    ";
}
#endif
