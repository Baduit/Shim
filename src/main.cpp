#include <regex>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdlib>

#include <replxx.hxx>
using Replxx = replxx::Replxx;

#include "CommandLineHandler.hpp"
#include "BashChild.hpp"
#include "ReplxxCallback.hpp"
#include "CallbacksData.hpp"

int main(int argc, char **argv)
{
	std::string shellCmd = "bash -sl";
	int nbHintLines = 8;
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

		if (((arg[0] == '-' && std::find(arg.begin(), arg.end(), 'h') != arg.end()) || (arg == "--hints")) && i + 1 < argc)
			nbHintLines = atoi(argv[i + 1]); // replace this with the c++ function from std

	}

	BashChild bashChild(shellCmd);
	std::string historyFilePath = secure_getenv("HOME");
	CommandLineHandler clh(historyFilePath + "/.shim_history", 1000, 1024, nbHintLines);
	CallbackData cbData(clh, bashChild, useHistory, useAllPaths);

	auto& completionData = cbData.getCompletionData();
	auto& regex_color = cbData.getColorations();

	clh.setCompletionCallback(hook_completion, static_cast<void*>(&completionData));
	clh.setHighlighterCallback(hook_color, static_cast<void*>(&regex_color));
	clh.setHintCallback(hook_hint, static_cast<void*>(&completionData));


	std::string input;
	clh.setPrompt(bashChild.getBashCurrentDir() + "$ ");
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
			clh.setPrompt(bashChild.getBashCurrentDir() + "$ ");
			continue;
		}
	}
	std::cout << std::endl << "Exiting Shim" << std::endl;
}