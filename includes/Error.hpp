#ifndef ERROR_HPP
# define ERROR_HPP

# include "Server.hpp"
# include <iostream>

std::string sputnbr(size_t n);

class serverError: public std::exception {
	public:
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

class HTTPError: public std::exception {
public:
	HTTPError(const char *context, const char *message, int error_code)
			: _error(std::string(context) + " : " + message),
			m_error_code(error_code) {
	}
	HTTPError(int error_code)
			: m_error_code(error_code) {
	}
	const char *what() const throw() {
		return _error.c_str();
	}
	int HTTPStatusCode() const {
		return m_error_code;
	}
	virtual ~HTTPError() throw() {}
private:
	std::string _error;
	int m_error_code;
};

#endif
