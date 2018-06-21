#pragma once

#include <string>
#include <vector>
#include <experimental/filesystem>
		

#include "CommandLineHandler.hpp"

namespace fs = std::experimental::filesystem;

class CallbackData
{
	using cl = Replxx::Color;
	using Colorations = std::vector<std::pair<std::string, cl>>;
	using Strings = std::vector<std::string>;

	public:
		CallbackData(CommandLineHandler& clh)
		{
			setExpressions(clh);
			setColors();
		}

		Colorations	getColorations() const { return _regexColor; }
		Strings		getKnownExpressions() const { return _knownExpressions; }

	private:
		void	setExpressions(CommandLineHandler& clh)
		{
			_knownExpressions = {
				"exit", "quit"
			};

			auto history = clh.getHistory();
			_knownExpressions.insert(_knownExpressions.end(), history.rbegin(), history.rend());
			
			auto binaries = getPathBinaries();
			_knownExpressions.insert(_knownExpressions.end(), binaries.rbegin(), binaries.rend());
		}

		Strings	getPathBinaries()
		{
			char* path = secure_getenv("PATH");
			Strings	paths;
			std::string pathToPush;
			while (path && *path)
			{
				if (*path == ':')
				{
					if (fs::exists(pathToPush))
						paths.push_back(pathToPush);
					pathToPush = "";
				}
				else
					pathToPush += *path;
				path++;
			}

			Strings binaries;
			for (const auto& p: paths)
			{
				try
				{
					for (auto& b: fs::directory_iterator(p))
					{
						auto perm = fs::status(b).permissions();
						if (((perm & fs::perms::owner_exec) != fs::perms::none)
							|| ((perm & fs::perms::group_exec) != fs::perms::none)
							|| ((perm & fs::perms::others_exec)) != fs::perms::none)
						{
							binaries.push_back(b.path().filename().string());
						}
					}
				}
				catch (...) {}
			}
			return binaries;
		}

		void	setColors()
		{
			_regexColor = {
					{"\\`", cl::BRIGHTCYAN},
					{"\\'", cl::BRIGHTBLUE},
					{"\\\"", cl::BRIGHTBLUE},
					{"\\-", cl::BRIGHTBLUE},
					{"\\+", cl::BRIGHTBLUE},
					{"\\=", cl::BRIGHTBLUE},
					{"\\/", cl::BRIGHTBLUE},
					{"\\*", cl::BRIGHTBLUE},
					{"\\^", cl::BRIGHTBLUE},
					{"\\.", cl::BRIGHTMAGENTA},
					{"\\(", cl::BRIGHTMAGENTA},
					{"\\)", cl::BRIGHTMAGENTA},
					{"\\[", cl::BRIGHTMAGENTA},
					{"\\]", cl::BRIGHTMAGENTA},
					{"\\{", cl::BRIGHTMAGENTA},
					{"\\}", cl::BRIGHTMAGENTA},

					{"color_black", cl::BLACK},
					{"color_red", cl::RED},
					{"color_green", cl::GREEN},
					{"color_brown", cl::BROWN},
					{"color_blue", cl::BLUE},
					{"color_magenta", cl::MAGENTA},
					{"color_cyan", cl::CYAN},
					{"color_lightgray", cl::LIGHTGRAY},
					{"color_gray", cl::GRAY},
					{"color_brightred", cl::BRIGHTRED},
					{"color_brightgreen", cl::BRIGHTGREEN},
					{"color_yellow", cl::YELLOW},
					{"color_brightblue", cl::BRIGHTBLUE},
					{"color_brightmagenta", cl::BRIGHTMAGENTA},
					{"color_brightcyan", cl::BRIGHTCYAN},
					{"color_white", cl::WHITE},
					{"color_normal", cl::NORMAL},

					{"[\\-|+]{0,1}[0-9]+", cl::YELLOW},
					{"[\\-|+]{0,1}[0-9]*\\.[0-9]+", cl::YELLOW},
					{"[\\-|+]{0,1}[0-9]+e[\\-|+]{0,1}[0-9]+", cl::YELLOW},

					{"\".*?\"", cl::BRIGHTGREEN},
					{"\'.*?\'", cl::BRIGHTGREEN},
				};
		}

	private:
		Colorations	_regexColor;
		Strings		_knownExpressions;
};