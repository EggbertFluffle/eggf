#pragma once

#include <vector>
#include <filesystem>

#define COLOR_GREY 8

#define PAIR_REGULAR 1
#define PAIR_REGULAR_HI 2
#define PAIR_DIRECTORY 3
#define PAIR_DIRECTORY_HI 4
#define PAIR_LINK 5
#define PAIR_LINK_HI 6
#define PAIR_IMAGE 7
#define PAIR_IMAGE_HI 8
#define PAIR_PATH 9
#define PAIR_PATH_HI 10

class App {
private:
	std::filesystem::path currentPath;
	std::filesystem::directory_entry currentDirectory;
	std::vector<std::filesystem::directory_entry> currentDirectoryContents;
	std::size_t previewScroll = 0;

	bool quit = false;
	std::size_t currentLine = 0;
	int maxx = 0;
	int maxy = 0;

	void Render() const;

	void PrintCurrentDirectory() const;
	void PreviewFile() const;
	void HandleKeyInput();
	void ReadCurrentDirectory();
	void InteractCurrentDirectoryEntry();

	void InteractDirectory(const std::filesystem::directory_entry& de);
	void InteractFile(const std::filesystem::directory_entry& de);

	void RenderDirectoryEntry(const std::filesystem::directory_entry& de, uint linePos, const bool highlight) const;

public:
	App();
	App(int argc, char** argv);

	~App();

	void Run();
};
