#include <regex>
#include <string>
#include <vector>
#include <iostream>

#include <replxx.hxx>
using Replxx = replxx::Replxx;

#include "CommandLineHandler.hpp"
#include "BashChild.hpp"
#include "ReplxxCallback.hpp"
#include "CallbacksData.hpp"

int main() {
	BashChild bashChild;
	CommandLineHandler clh;
	CallbackData cbData(clh);

	auto knownExpressions = cbData.getKnownExpressions();
	auto regex_color = cbData.getColorations();

	clh.setCompletionCallback(hook_completion, static_cast<void*>(&knownExpressions));
	clh.setHighlighterCallback(hook_color, static_cast<void*>(&regex_color));
	clh.setHintCallback(hook_hint, static_cast<void*>(&knownExpressions));


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
	std::cout << std::endl << "Exiting Shimon" << std::endl;

	return 0;
}