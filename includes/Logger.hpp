#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>

class Logger {
	Logger();
	~Logger();
	Logger(Logger const &);
	Logger&	operator=(Logger const &);

public:
	static Logger const &	Log();

	template <typename T>
	Logger const &	operator<<(T const & val) const {
#ifdef LOG
		std::cout << val;
#else
		(void) val;
#endif
		return *this;
	}

	Logger const &	operator<<(std::ostream & (*fn)(std::ostream &)) const;
};

#endif
