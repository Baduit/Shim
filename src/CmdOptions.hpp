#pragma once

#include <set>
#include <regex>
#include <string>
#include <cstdlib>
#include <fstream>
#include <thread>
#include <mutex>


class CmdOptions
{
	public:
		CmdOptions() = default;

		void					operator()(const std::string& cmdName)
		{
			if (cmdName == getName())
				return;
			_name = cmdName;

			if (_name.find("(") != std::string::npos || _name.find(")") != std::string::npos)
				return;

			if (_name.find("\"") != std::string::npos || _name.find("'") != std::string::npos || _name.find("`") != std::string::npos)
				return;
			
			_thread = std::thread([&]{
				system(std::string("man 1 " + this->getName() + " 2>/dev/null | grep - > /tmp/shim_man.txt\n").c_str());
				std::ifstream pwdFile("/tmp/shim_man.txt");

				std::string oriStr;;
				while (std::getline(pwdFile, oriStr))
				{
					size_t pos = 0;
					std::string regexStr = oriStr;
					std::smatch match;

					while(std::regex_search(regexStr, match, std::regex("\\B\\-\\w+")))
					{
						std::string c = match[0];
						pos += std::string(match.prefix()).size();

						if (pos > 0 && oriStr[pos - 1] == '-')
							c = "-" + c;
						this->insert(c);

						pos += c.size();
						regexStr = match.suffix();
					}
				}
			});
			_thread.detach();
		}

		void 					insert(const std::string& str)
		{
			std::string s = str;
			std::lock_guard lock(_mutex);
			if (str[0] == str[1])
			{
				s.erase(0, 2);
				_optionsDouble.insert(s);
			}
			else
			{
				s.erase(0, 1);
				_optionsMono.insert(s);
			}
		}

		auto	get()
		{
			std::lock_guard lock(_mutex);
			auto copy = std::make_pair(_optionsMono, _optionsDouble);
			return copy;
		}

		const std::string& getName() const { return _name; }

	private:
		std::string				_name;
		std::mutex				_mutex;
		std::set<std::string>	_optionsMono;
		std::set<std::string>	_optionsDouble;
		std::thread				_thread;
};