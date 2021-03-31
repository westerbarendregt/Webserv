#include "Logger.hpp"

Logger::Logger() {}
Logger::~Logger() {}
Logger::Logger(Logger const &) {}
Logger&	Logger::operator=(Logger const &) { return *this; }

Logger const &	Logger::Log() {
	static Logger	logger;
	return logger;
}

Logger const &	Logger::operator<<(std::ostream & (*fn)(std::ostream &)) const {
#ifdef LOG
	fn(std::cout);
#else
	(void) fn;
#endif
	return *this;
}
