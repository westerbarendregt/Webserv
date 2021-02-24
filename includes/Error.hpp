#ifndef ERROR_HPP
# define ERROR_HPP

# include "Server.hpp"
# include <iostream>

std::string sputnbr(size_t n);

class serverError: public std::exception {
	public:
		serverError(const char *path, size_t n) //for parseError
			: _error(std::string(path) + ": syntax error line " + sputnbr(n)) {
		}
		serverError(const char *context, const char *message) 
			: _error(std::string(context) + " : " + message) {
		}
		const char *what() const throw() {
			return _error.c_str();
		}
		virtual ~serverError() throw() {}
	private:
		std::string _error;
};

#endif
