#pragma once

#include <string>
#include <vector>
#include <experimental/filesystem>
		

#include "CommandLineHandler.hpp"

namespace fs = std::experimental::filesystem;

using Strings = std::vector<std::string>;

struct CompletionData
{
	CompletionData(BashChild& bc): bashChild(bc) {}

	BashChild&	bashChild;
	Strings		history;
	Strings		aliases;
	Strings		binaries;
};

enum BinariesPathCompletion
{
	NORMAL,
	ALL,
	NONE
};

class CallbackData
{
	using cl = Replxx::Color;
	using Colorations = std::vector<std::pair<std::string, cl>>;

	public:
		CallbackData(CommandLineHandler& clh, BashChild& bashChild, bool useHistory = true, BinariesPathCompletion useAllPaths = NORMAL): _completionData(bashChild)
		{
			setExpressions(clh, useHistory, useAllPaths);
			setColors();
		}

		Colorations			getColorations() const { return _regexColor; }
		CompletionData		getCompletionData() const { return _completionData; }

	private:
		void	setExpressions(CommandLineHandler& clh, bool useHistory, bool useAllPaths)
		{
			_completionData.binaries = {
				"exit", "quit"
			};

			if (useHistory)
			{
				_completionData.history = clh.getHistory();
			}

			_completionData.aliases = _completionData.bashChild.getBashAliases();

			Strings paths;
			if (useAllPaths == BinariesPathCompletion::NORMAL)
				paths = {"/bin", "/usr/bin"};
			else if (useAllPaths == BinariesPathCompletion::ALL) 
				paths = getEnvPath();
			auto binaries = getPathBinaries(paths);
			_completionData.binaries.insert(_completionData.binaries.end(), binaries.rbegin(), binaries.rend());
		}

		Strings	getEnvPath()
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
			return paths;
		}

		Strings	getPathBinaries(const Strings& paths)
		{
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
		CompletionData		_completionData;
		Colorations			_regexColor;
};