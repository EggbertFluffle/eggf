#include <filesystem>
#include <fstream>
#include <ncurses.h>
#include <string>
#include <algorithm>

#include "config.hpp"
#include "App.hpp"
#include "DirectoryEntry.hpp"
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

	init_color(COLOR_WHITE, 999, 999, 999);
	init_color(COLOR_GREY, 255, 255, 255);

	init_pair(PAIR_REGULAR, COLOR_WHITE, COLOR_BLACK);
	init_pair(PAIR_REGULAR_HI, COLOR_WHITE, COLOR_GREY);

	init_pair(PAIR_DIRECTORY, COLOR_BLUE, COLOR_BLACK);
	init_pair(PAIR_DIRECTORY_HI, COLOR_BLUE, COLOR_GREY);

	init_pair(PAIR_PATH, COLOR_YELLOW, COLOR_BLACK);
	init_pair(PAIR_PATH_HI, COLOR_YELLOW, COLOR_GREY);

	EggLog::Push("This is a test log");
	EggLog::Push("This is a test log 2");
	EggLog::Push("This is a test log 3");
}

App::~App() { endwin(); }

void App::Run() {
	ReadCurrentDirectory();
	PrintCurrentDirectory();
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
	MEVENT event;
	if(getmouse(&event) == OK) {
		EggLog::Push(std::string("Mouse X: ") + std::to_string(event.x));
		EggLog::Push("Hello world!");
		bool mainScroll = event.x < (getmaxx(stdscr) / 2);
		if(event.bstate & BUTTON4_PRESSED) {
			if(mainScroll) {
				if(currentLine < currentDirectoryContents.size() - 1) currentLine++;
			} else {
				previewScroll++;
			}
			return;
		} else if(event.bstate & BUTTON5_PRESSED) {
			if(mainScroll) {
				if(currentLine != 0) currentLine--;
			} else {
				if(previewScroll > 0) previewScroll--;
			}
			return;
		}
	}
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
	const DirectoryEntry& directoryEntry = currentDirectoryContents[currentLine];

	if(directoryEntry.entry.is_directory()) {
		currentPath /= directoryEntry.entry.path(); 
		currentDirectory = std::filesystem::directory_entry(currentPath);
		currentLine = 0;
		ReadCurrentDirectory();
	} else if(directoryEntry.entry.is_regular_file()) {
		std::string command = USER_CONFIG::EDITOR;

		// If command has an extension, match the correct command for the extension from config
		// User USER_CONFIG::EDITOR as a fallback if file handler doesn't exist
		if(directoryEntry.entry.path().has_extension()) {
			const std::string fileExtension = directoryEntry.entry.path().extension();
			for(const std::pair<std::vector<std::string>, std::string>& handler : USER_CONFIG::FILE_HANDLERS) {
				if(std::count(handler.first.cbegin(), handler.first.cend(), fileExtension) > 0) {
					command = handler.second;
				}
			}
		}

		const std::size_t pathArgIndex = command.find_first_of("%s");
		command.replace(pathArgIndex, 2, directoryEntry.entry.path().string());
		system(command.c_str());
	}
}

void App::ReadCurrentDirectory() {
	const std::filesystem::directory_iterator currDirItr(currentPath);
	currentDirectoryContents.clear();

	uint linePos = 0;
	for(std::filesystem::directory_entry it : currDirItr) {
		currentDirectoryContents.push_back(DirectoryEntry(it, linePos));
		linePos++;
	}
}

void App::PrintCurrentDirectory() const {
	box(stdscr, 0, 0);
	attron(COLOR_PAIR(PAIR_PATH));
	mvwprintw(stdscr, 0, 2, " %s ", currentPath.c_str());
	attroff(COLOR_PAIR(PAIR_PATH));
	for(const DirectoryEntry& entry : currentDirectoryContents) {
		entry.Render(currentLine);
	}
}

void App::PreviewFile() const {
	// TODO:
	// Add a preview for directories
	// probably have to abstract printing current directory

	if(!currentDirectoryContents[currentLine].entry.is_regular_file()) return;
	const int maxy = getmaxy(stdscr);
	std::fstream file(currentDirectoryContents[currentLine].entry.path().string());
	const std::size_t previewStart = getmaxx(stdscr) / 2;
	if(!file.is_open()) mvprintw(1, previewStart + 1, "There was an error opening the file");

	std::string line;
	int linePos = 1;
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

	// if(previewStart < 20) return;
	move(1, previewStart);
	vline(ACS_VLINE, maxy - 1 - (EggLog::Empty() ? 0 : 1 + EggLog::Count()));
	mvaddch(maxy - 1 - (EggLog::Empty() ? 0 : 1 + EggLog::Count()), previewStart, ACS_SSBS);
	mvaddch(0, previewStart, ACS_BSSS);
}
