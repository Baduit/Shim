#include <utility>
#include <string>
#include <vector>
#include <algorithm>
#include <experimental/filesystem>
#include <fstream>

#include "CallbacksData.hpp"
#include "BashParser.hpp"

namespace fs = std::experimental::filesystem;

template<typename Container>
void emplaceBackToContainer(Container& c, const std::string& str)
{
	if (std::find(c.begin(), c.end(), str) == c.end())
		c.emplace_back(str);
}

// ajouter 2 call backs, une sur le context et une sur le préfix
template<typename C, typename CbPrefix, typename CbContext>
C completion(const std::string& context, const std::string& prefix, CompletionData* data, CbPrefix&& cbPrefix, CbContext&& cbContext)
{
	C c;

	BashRole bashRole = BashParser()(context);

	if (bashRole == BashRole::LOCAL_BIN)
	{
		for (auto& p: fs::directory_iterator(data->bashChild.getBashCurrentDir()))
		{
			std::string filename = p.path().filename().string();
			if (filename.compare(0, prefix.size(), prefix) == 0)
				cbPrefix(filename, c);
		}

		// or if there is a path after the ./ use this path instead example ./../ TODO
	}

	for (const auto& e : data->history)
	{
		if (e.compare(0, context.size(), context) == 0)
			cbContext(e, c);
		if (prefix.size() >= 2 || (!prefix.empty()))
			if (e.compare(0, prefix.size(), prefix) == 0)
				cbPrefix(e, c);
	}
   	
	if (bashRole == BashRole::PATH_BIN)
	{
		for (const auto& e : data->binaries)
		{
			if (prefix.size() >= 2 || (!prefix.empty()))
				if (e.compare(0, prefix.size(), prefix) == 0)
					cbPrefix(e, c);
		}
		// use aliases TODO
	}

	if (bashRole == BashRole::ARG)
	{
		for (auto& p: fs::directory_iterator(data->bashChild.getBashCurrentDir()))
		{
			std::string filename = p.path().filename().string();
			if (filename.compare(0, prefix.size(), prefix) == 0)
				cbPrefix(filename, c);
		}
		// or if there is a path after use this path TODO
		// read the man of the command uses if it begins with a - (if it exists) and extract the possibles options TODO
	}

	return c;
}

inline Replxx::completions_t hook_completion(const std::string& context, int index, void* user_data)
{
	auto* data = static_cast<CompletionData*>(user_data);
	Replxx::completions_t completions;

	std::string prefix {context.substr(index)};

	auto prefixCallback = [&](const std::string& str, auto& container){
		container.emplace_back(str);
	};

	auto contextCallback = [&](const std::string& str, auto& container){
		container.emplace_back(str.substr(context.size() - prefix.size()));
	};

	completions = completion<Replxx::completions_t>(context, prefix, data, prefixCallback, contextCallback);


	return completions;
}

inline Replxx::hints_t hook_hint(const std::string& context, int index, Replxx::Color& color, void* user_data)
{
	auto* data = static_cast<CompletionData*>(user_data);
	Replxx::hints_t hints;

	if (context.empty())
		return hints;

	std::string prefix = context.substr(index);

	auto prefixCallback = [&](const std::string& str, auto& container){
		emplaceBackToContainer(container, str.substr(prefix.size()));
	};

	auto contextCallback = [&](const std::string& str, auto& container){
		emplaceBackToContainer(container, str.substr(context.size()));
	};

	hints = completion<Replxx::hints_t>(context, prefix, data, prefixCallback, contextCallback);

	if (hints.size() == 1)
		color = Replxx::Color::GREEN;

	return hints;
}

inline void hook_color(std::string const& context, Replxx::colors_t& colors, void* user_data)
{
	auto* regex_color = static_cast<std::vector<std::pair<std::string, Replxx::Color>>*>(user_data);

	for (auto const& e : *regex_color)
	{
		size_t pos {0};
		std::string str = context;
		std::smatch match;

		while(std::regex_search(str, match, std::regex(e.first)))
		{
			std::string c {match[0]};
			pos += std::string(match.prefix()).size();

			for (size_t i = 0; i < c.size(); ++i)
				colors.at(pos + i) = e.second;

			pos += c.size();
			str = match.suffix();
		}
	}
}