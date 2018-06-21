#pragma once

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdexcept>
#include <sstream>
#include <signal.h>
#include <string>

class BashChild
{
	public:
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
				system(shell.c_str());
				close(_pipefd[0]);
				exit(EXIT_SUCCESS);
				close(_pipefd[0]);
			}
			else
			{
				close(_pipefd[0]);
			}
		}

		~BashChild() { waitExit(); }

		BashChild(const BashChild&) = delete;
		BashChild(BashChild&&) = default;

		BashChild& 		operator=(const BashChild&) = delete;
		BashChild& 		operator=(BashChild&&) = default;

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

		void			waitEndBashCommand()
		{
			*this << "kill -s SIGUSR1 " << getpid() <<" \n";
			pause();
		}

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

		const pid_t		getCpid() const { return _cpid; }
	private:
		pid_t	_cpid;
		int		_pipefd[2];
};