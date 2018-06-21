#pragma once

#include <algorithm>

#include "replxx.hxx"

class CommandLineHandler
{
	using Replxx = replxx::Replxx;
	using Strings = std::vector<std::string>;

	public:
		CommandLineHandler(const std::string& historyFile, int maxHistorySize, int maxLineSize, int maxHintRows): _historyFile(historyFile)
		{
			_rx.install_window_change_handler();
			_rx.history_load(historyFile);
			_rx.set_max_history_size(maxHistorySize);
			_rx.set_max_line_size(maxLineSize);
			_rx.set_max_hint_rows(maxHintRows);
		}

		virtual ~CommandLineHandler() { _rx.history_save(_historyFile); }

		bool				operator>>(std::string& result)
		{
			char const* cinput = nullptr;

			do
			{
				cinput = _rx.input(_prompt);
			}
			while ((cinput == nullptr) && (errno == EAGAIN));

			if (cinput == nullptr)
				return false;
			result = cinput;
			return true;
		}

		void				setPrompt(const std::string& newPrompt) { _prompt = newPrompt; }
		const std::string&	getPrompt() const { return _prompt; }

		void				addToHistory(const std::string& cmd) { _rx.history_add(cmd); }
		Strings				getHistory()
		{
			Strings	history;
			int historySize = _rx.history_size();
			for (int i = 0; i < historySize; ++i)
				if (std::find(history.begin(), history.end(), _rx.history_line(i)) == history.end())
					history.push_back(_rx.history_line(i));
			return history;
		}

		void				clearScreen() { _rx.clear_screen(); }


		void 				setCompletionCallback(Replxx::completion_callback_t const& fn, void* userData)
		{
			_rx.set_completion_callback(fn, userData);
		}

		void 				setHighlighterCallback(Replxx::highlighter_callback_t const& fn, void* userData)
		{
			_rx.set_highlighter_callback(fn, userData);
		}

		void 				setHintCallback(Replxx::hint_callback_t const& fn, void* userData)
		{
			_rx.set_hint_callback(fn, userData);
		}

	private:
		Replxx		_rx;
		std::string	_prompt = "\x1b[1;32mreplxx\x1b[0m> ";
		std::string	_historyFile;
};