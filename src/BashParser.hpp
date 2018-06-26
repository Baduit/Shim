#pragma once

#include <string>
#include <utility>
#include <vector>
#include <algorithm>

enum BashRole
{
	PATH_BIN = 1,
	LOCAL_BIN = 2,
	ARG = 3,
	ARG_OPTION = 4,
	UNKNOWN = 0
};

class BashParser
{
	using Chunk = std::pair<std::string, std::string>;
	public:
		struct Answer
		{
			Answer() = default;
			Answer(BashRole br): bashRole(br) {}
			Answer(BashRole br, const std::string& p): bashRole(br), path(p) {}
			Answer(BashRole br, const std::string& p, const std::string& b): bashRole(br), path(p), bin(b) {}

			BashRole	bashRole = BashRole::UNKNOWN;
			std::string	path;
			std::string	bin;
		};
	public:
		BashParser() = default;

		Answer	operator()(const std::string& line)
		{
			auto chunks = splitLineToChunks(line);
			auto cmd = chunksToLastCmd(chunks);

			if (cmd.size() <= 1)
			{
				if (cmd.size() == 0)
					return Answer(BashRole::PATH_BIN);

				auto str = cmd.front();
				if (str.size() >= 2 && str[0] == '.' && str[1] == '/')
				{
					std::string path = str;
					while (!path.empty() && path.back() != '/')
						path.pop_back();
					return Answer(BashRole::LOCAL_BIN, path);
				}
				else
				{
					return Answer(BashRole::PATH_BIN, "", str);
				}
			}
			else
			{
				auto str = cmd.back();
				if (str.front() == '-')
					return Answer(BashRole::ARG_OPTION, "", cmd.front());

				std::string path = str;
				while (!path.empty() && path.back() != '/')
					path.pop_back();
				return Answer(BashRole::ARG, path);
			}
			return Answer(BashRole::UNKNOWN);
		}

	private:
		std::vector<std::string> chunksToLastCmd(const std::vector<Chunk>& chunks)
		{
			std::vector<std::string> cmdSep =
				{";", "&&","||",">",">>","<","<<"};
			std::vector<std::string> cmd;

			for (const auto& [str, sep]: chunks)
			{
				if (!str.empty())
					cmd.push_back(str);
				if (std::find(cmdSep.begin(), cmdSep.end(), sep) != cmdSep.end())
					cmd.clear();
			}
			return cmd;
		}

		std::vector<Chunk> splitLineToChunks(const std::string& line)
		{
			std::vector<std::pair<std::string, std::string>> chunks;
			std::string currentChunk;
			bool isInString = false;

			for (const auto& c: line)
			{
				if (isCharStringDelim(c))
					isInString = !isInString;
				if (isCharaterDelimiter(c) && !isInString)
				{
					{
						std::string sepString;
						sepString += c;
						chunks.push_back(std::make_pair(currentChunk, sepString));
						currentChunk = "";
					}
				}
				else
					currentChunk += c;
			}
			chunks.push_back(std::make_pair(currentChunk, ""));
			return chunks;
		}
		
		bool	isCharaterDelimiter(char c) const
		{
			std::string	delimiters = "\n\r \t&|<>;";
			for (auto d: delimiters)
				if (!c || c == d)
					return true;
			return false;
		}

		bool	isCharStringDelim(char c) { return (c == '\'' || c == 34 || c == '`') ? true : false; }
};