#include <set>
#include <unordered_set>
#include <utility>
#include <string>
#include <vector>
#include <algorithm>


inline Replxx::completions_t hook_completion(std::string const& context, int index, void* user_data)
{
	auto* examples = static_cast<std::vector<std::string>*>(user_data);
	Replxx::completions_t completions;

	std::string prefix {context.substr(index)};
	for (auto const& e : *examples)
	{
		if (e.compare(0, context.size(), context) == 0)
			completions.emplace_back(e.substr(context.size() - prefix.size()));
		if (e.compare(0, prefix.size(), prefix) == 0)
			completions.emplace_back(e);
	}

	return completions;
}

template<typename Container>
void emplaceBackToContainser(Container& c, const std::string& str)
{
	if (std::find(c.begin(), c.end(), str) == c.end())
		c.emplace_back(str);
}

inline Replxx::hints_t hook_hint(std::string const& context, int index, Replxx::Color& color, void* user_data) {
	auto* examples = static_cast<std::vector<std::string>*>(user_data);
	Replxx::hints_t hints;

	if (context.empty())
		return hints;

	std::string prefix {context.substr(index)};
	for (const auto& e : *examples)
	{
		if (e.compare(0, context.size(), context) == 0)
			emplaceBackToContainser(hints, e.substr(context.size()));
		if (prefix.size() >= 2 || (!prefix.empty()))
			if (e.compare(0, prefix.size(), prefix) == 0)
				emplaceBackToContainser(hints, e.substr(prefix.size()));
	}

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