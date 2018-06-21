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
		CallbackData(CommandLineHandler& clh, bool useHistory = true, bool useAllPaths = false)
		{
			setExpressions(clh, useHistory, useAllPaths);
			setColors();
		}

		Colorations	getColorations() const { return _regexColor; }
		Strings		getKnownExpressions() const { return _knownExpressions; }

	private:
		void	setExpressions(CommandLineHandler& clh, bool useHistory, bool useAllPaths)
		{
			_knownExpressions = {
				"exit", "quit"
			};

			if (useHistory)
			{
				auto history = clh.getHistory();
				_knownExpressions.insert(_knownExpressions.end(), history.rbegin(), history.rend());
			}

			if (useAllPaths) 
			{
				auto binaries = getPathBinaries();
				_knownExpressions.insert(_knownExpressions.end(), binaries.rbegin(), binaries.rend());
			}
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

					{"\\>", cl::RED},
					{"\\<", cl::RED},
					{"\\|", cl::RED},
					{"\\;", cl::RED},
					{"\\&", cl::RED},

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