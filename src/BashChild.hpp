#pragma once

#include <unistd.h> // the goal is to remove this header because it not cross platorm
#include <fcntl.h>
#include <sys/wait.h>
#include <stdexcept>
#include <sstream>
#include <signal.h>
#include <string>
#include <fstream>
#include <vector>

#include <boost/process.hpp>

namespace bp = boost::process;

class BashChild
{
	public:
		// boost process
		BashChild(const std::string& shell = "bash -sl")
		{
			if (pipe(_pipefd) == -1)
				throw std::runtime_error("Pipe failed");
			
			signal(SIGUSR1, [](int){});
			if ((_cpid = fork()) == -1)
				throw std::runtime_error("Fork failed");
			
			if (_cpid != 0)
			{
				close(_pipefd[1]);
				close(0);
				dup2(_pipefd[0], 0);
				std::system(shell.c_str());
				close(_pipefd[0]);
				exit(EXIT_SUCCESS);
			}
			else
			{
				close(_pipefd[0]);
			}
		}

		// boost process
		~BashChild() { waitExit(); }

		BashChild(const BashChild&) = delete;
		BashChild(BashChild&&) = default;

		BashChild& 		operator=(const BashChild&) = delete;
		BashChild& 		operator=(BashChild&&) = default;

		// boost process
		void			writeToBash(const std::string& message)
		{
			write(_pipefd[1], message.c_str(), message.length());
		}

		template<typename T>
		BashChild&		operator<<(const T& message)
		{
			std::stringstream ss;
			ss << message;
			writeToBash(ss.str());
			return *this;
		}

		// create a little helper program in c++ to make this portable because this won't work on powershell & cmd.exe
		void			waitEndBashCommand()
		{
			*this << "kill -s SIGUSR1 " << getpid() <<" \n";
			pause();
		}

		// boost process
		int				waitExit()
		{
			if (_cpid <= 0)
				return -1;
			int bashExitStatus = 0;
			close(_pipefd[1]);
			if (waitpid(_cpid, &bashExitStatus, 0) == -1)
				return -1;
			return bashExitStatus;
		}

		// create a little helper program in c++ to make this portable because this won't work on powershell & cmd.exe
		// use env variable instead of tmp_file would be better i think
		std::string		getBashCurrentDir()
		{
			(*this) << "pwd > /tmp/shim_pwd.txt\n";
			waitEndBashCommand();
			std::ifstream pwdFile("/tmp/shim_pwd.txt");
			std::string path;
			std::getline(pwdFile, path);
			return path;
		}

		// add a try catch in case this does not work, improve the parsing to make this work on powershell
		// use env variable instead of tmp_file would be better i think
		std::vector<std::string>		getBashAliases()
		{
			(*this) << "alias > /tmp/shim_aliases.txt\n";
			waitEndBashCommand();
			std::ifstream pwdFile("/tmp/shim_aliases.txt");
			std::vector<std::string> aliases;
			std::string alias;
			while (std::getline(pwdFile, alias))
				aliases.push_back(alias.substr(6, alias.find("=") - 6));
			return aliases;
		}

		// won't be need with boost process
		const pid_t		getCpid() const { return _cpid; }
	private:
		pid_t	_cpid;
		int		_pipefd[2];
};