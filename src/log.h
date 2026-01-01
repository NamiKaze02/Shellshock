#pragma once

#include <string>
#include <vector>

namespace TGW {
enum class LogType { INFO };

struct LogEntry {
	std::string message;
	LogType type;

	const char *GetTypeString() const
	{
		switch (type) {
		case LogType::INFO:
			return "[INFO] ";
		default:
			return "[UNKNOWN] ";
		}
	}

	inline const std::string AsString() const { return GetTypeString() + message; }
};

class Logger {
  public:
	inline static void AddLog(const std::string &fmt, LogType type) { Get()._items.push_back({fmt, type}); }
	inline static void Clear() { Get()._items.clear(); }
	inline static const std::vector<LogEntry> &GetAll() { return Get()._items; }

  private:
	inline static Logger &Get()
	{
		static Logger instance;
		return instance;
	}
	std::vector<LogEntry> _items;
};
} // namespace TGW