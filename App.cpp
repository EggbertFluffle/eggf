#include <filesystem>
#include <fstream>
#include <ncurses.h>
#include <string>
#include <algorithm>

#include "config.hpp"
#include "App.hpp"
#include "EggLog.hpp"

App::App():
	currentPath(std::filesystem::current_path())
{};

App::App(int argc, char** argv):
	currentPath(argc > 1 ? std::filesystem::absolute(argv[1]) : std::filesystem::current_path())
{
	// Initialize ncurses
	initscr();
	noecho();
	start_color();
	keypad(stdscr, TRUE);
	mouseinterval(0);
	getmaxyx(stdscr, maxy, maxx);

	//Initialize colors, make this an option for the config
	init_color(COLOR_WHITE, 999, 999, 999);
	init_color(COLOR_GREY, 255, 255, 255);

	init_pair(PAIR_REGULAR, COLOR_WHITE, COLOR_BLACK);
	init_pair(PAIR_REGULAR_HI, COLOR_WHITE, COLOR_GREY);

	init_pair(PAIR_DIRECTORY, COLOR_BLUE, COLOR_BLACK);
	init_pair(PAIR_DIRECTORY_HI, COLOR_BLUE, COLOR_GREY);

	init_pair(PAIR_PATH, COLOR_YELLOW, COLOR_BLACK);
	init_pair(PAIR_PATH_HI, COLOR_YELLOW, COLOR_GREY);
}

App::~App() { endwin(); }

void App::Run() {
	ReadCurrentDirectory();
	PrintCurrentDirectory();

	// Main application loop
	while(!quit) {
		HandleKeyInput();
		Render();
	}
}

void App::Render() const {
	erase();
	PrintCurrentDirectory();
	EggLog::Render();
	if(USER_CONFIG::FILE_PREVIEW) PreviewFile();
	move(currentLine + 1, 1);
	refresh();
}

void App::HandleKeyInput() {
	// Handle mouse events before key events
	// TODO: DOESNT WORK
	// MEVENT event;
	// if(getmouse(&event) == OK) {
	// 	EggLog::Push(std::string("Mouse X: ") + std::to_string(event.x));
	// 	EggLog::Push("Hello world!");
	// 	bool mainScroll = event.x < (getmaxx(stdscr) / 2);
	// 	if(event.bstate & BUTTON4_PRESSED) {
	// 		if(mainScroll) {
	// 			if(currentLine < currentDirectoryContents.size() - 1) currentLine++;
	// 		} else {
	// 			previewScroll++;
	// 		}
	// 		return;
	// 	} else if(event.bstate & BUTTON5_PRESSED) {
	// 		if(mainScroll) {
	// 			if(currentLine != 0) currentLine--;
	// 		} else {
	// 			if(previewScroll > 0) previewScroll--;
	// 		}
	// 		return;
	// 	}
	// }

	// Keybind actions
	// TODO: Make configurable
	switch(getch()) {
		case 'q':
			quit = true;
			break;
		case KEY_DOWN:
		case 'j':
			if(currentLine < currentDirectoryContents.size() - 1) currentLine++;
			break;
		case KEY_UP:
		case 'k':
			if(currentLine != 0) currentLine--;
			break;
		case KEY_LEFT:
		case KEY_BACKSPACE:
		case 'h':
			currentPath = currentPath.parent_path();
			currentDirectory = std::filesystem::directory_entry(currentPath);
			currentLine = 0;
			ReadCurrentDirectory();
			break;
		case KEY_RIGHT:
		case '\n': 
		case 'l':
			InteractCurrentDirectoryEntry();
			break;
	}
}

void App::InteractCurrentDirectoryEntry() {
	const std::filesystem::directory_entry& de = currentDirectoryContents[currentLine];
		
	if(de.is_directory()) {
		InteractDirectory(de);
	} else if(de.is_regular_file()) {
		InteractFile(de);
	}
}

void App::InteractDirectory(const std::filesystem::directory_entry& de) {
	currentPath /= de.path(); 
	currentDirectory = std::filesystem::directory_entry(currentPath);
	currentLine = 0;
	ReadCurrentDirectory();
}

void App::InteractFile(const std::filesystem::directory_entry& de) {
	std::string command = USER_CONFIG::EDITOR;

	// If command has an extension, match the correct command for the extension from config
	// User USER_CONFIG::EDITOR as a fallback if file handler doesn't exist
	if(de.path().has_extension()) {
		const std::string fileExtension = de.path().extension();
		for(const std::pair<std::vector<std::string>, std::string>& handler : USER_CONFIG::FILE_HANDLERS) {
			if(std::count(handler.first.cbegin(), handler.first.cend(), fileExtension) > 0) {
				command = handler.second;
			}
		}
	}

	const std::size_t pathArgIndex = command.find_first_of("%s");
	command.replace(pathArgIndex, 2, de.path().string());
	system(command.c_str());
}

void App::ReadCurrentDirectory() {
	const std::filesystem::directory_iterator currDirItr(currentPath);
	currentDirectoryContents.clear();

	uint linePos = 0;
	for(std::filesystem::directory_entry it : currDirItr) {
		currentDirectoryContents.push_back(std::filesystem::directory_entry(it));
		linePos++;
	}

	std::sort(currentDirectoryContents.begin(), currentDirectoryContents.end(), [](std::filesystem::directory_entry& a, std::filesystem::directory_entry& b){
		if(a.is_directory() == b.is_directory()) {
			return a.path().stem().string() < b.path().stem().string();
		}
		return !(a.is_regular_file() && b.is_directory());
	});
}

void App::PrintCurrentDirectory() const {
	box(stdscr, 0, 0);

	// Print current path on top left of screen
	attron(COLOR_PAIR(PAIR_PATH));
	mvwprintw(stdscr, 0, 2, " %s ", currentPath.c_str());
	attroff(COLOR_PAIR(PAIR_PATH));

	// Print all directory entries in the current directory
	uint linePos = 0;
	for(const std::filesystem::directory_entry& entry : currentDirectoryContents) {
		RenderDirectoryEntry(entry, linePos, currentLine == linePos);
		linePos++;
	}
}

void App::RenderDirectoryEntry(const std::filesystem::directory_entry& de, uint linePos, const bool highlight) const {
	const int colorPair = (de.is_directory() ? PAIR_DIRECTORY : PAIR_REGULAR) + (highlight ? 1 : 0);
	attron(COLOR_PAIR(colorPair));
	mvprintw(linePos + 1, 2, "%s", de.path().filename().c_str());
	attroff(COLOR_PAIR(colorPair));
}

void App::PreviewFile() const {
	// TODO:
	// Add a preview for directories
	// probably have to abstract printing current directory

	if(!currentDirectoryContents[currentLine].is_regular_file()) return;
	std::fstream file(currentDirectoryContents[currentLine].path().string());
	const std::size_t previewStart = getmaxx(stdscr) / 2;
	if(!file.is_open()) {
		std::string err = std::string("There was an error opening the file");
		err.resize(previewStart - 1);
		mvprintw(1, previewStart + 1, "%s", err.c_str());
	}

	std::string line;
	std::size_t linePos = 1;

	// Skip lines that should be scrolled past
	for(std::size_t i = 0; i < previewScroll; i++) {
		getline(file, line);
	}

	while(getline(file, line) && linePos + 1 < maxy - 1 - EggLog::Count()) { 
		// Remove leading tabs
		if(!line.empty()) {
			uint8_t tabCount = 0;
			while(*line.cbegin() == '\t') {
				tabCount++;
				line.erase(line.begin());
			}
			while(tabCount != 0) {
				line.insert(0, USER_CONFIG::TAB_REPLACE);
				tabCount--;
			}
		}

		if(line.size() > previewStart - 2) line.resize(previewStart - 2);
		mvprintw(linePos, previewStart + 1, "%s", line.c_str());
		linePos++;
    }

	// Draw outter lines for preview
	move(1, previewStart);
	vline(ACS_VLINE, maxy - 1 - (EggLog::Empty() ? 0 : 1 + EggLog::Count()));
	mvaddch(maxy - 1 - (EggLog::Empty() ? 0 : 1 + EggLog::Count()), previewStart, ACS_SSBS);
	mvaddch(0, previewStart, ACS_BSSS);
}
