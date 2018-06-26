#include <regex>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include <replxx.hxx>
using Replxx = replxx::Replxx;

#include "CommandLineHandler.hpp"
#include "BashChild.hpp"
#include "ReplxxCallback.hpp"
#include "CallbacksData.hpp"

int main(int argc, char **argv)
{
	std::string shellCmd = "bash -sl";
	bool useHistory = true;
	BinariesPathCompletion useAllPaths = BinariesPathCompletion::NORMAL;

	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];

		if (((arg[0] == '-' && std::find(arg.begin(), arg.end(), 's') != arg.end()) || (arg == "--shell")) && i + 1 < argc)
			shellCmd = argv[i + 1];

		if ((arg[0] == '-' && std::find(arg.begin(), arg.end(), 'N') != arg.end()) || (arg == "--no-history"))
			useHistory = false;
		if (arg == "--paths=none")
			useAllPaths = BinariesPathCompletion::NONE;
		if (arg == "--paths=all")
			useAllPaths = BinariesPathCompletion::ALL;

	}

	BashChild bashChild(shellCmd);
	std::string historyFilePath = secure_getenv("HOME");
	CommandLineHandler clh(historyFilePath + "/.shim_history", 1000, 256, 5);
	CallbackData cbData(clh, bashChild, useHistory, useAllPaths);

	auto completionData = cbData.getCompletionData();
	auto regex_color = cbData.getColorations();

	clh.setCompletionCallback(hook_completion, static_cast<void*>(&completionData));
	clh.setHighlighterCallback(hook_color, static_cast<void*>(&regex_color));
	clh.setHintCallback(hook_hint, static_cast<void*>(&completionData));


	std::string input;
	while (clh >> input)
	{
		if (input.empty())
			continue;
		else if (input.compare(0, 4, "quit") == 0 || input.compare(0, 4, "exit") == 0)
		{
			clh.addToHistory(input);
			break;
		}
		else
		{
			bashChild << input << "\n";
			bashChild.waitEndBashCommand();
			clh.addToHistory(input);
			continue;
		}
	}
	std::cout << std::endl << "Exiting Shim" << std::endl;
}