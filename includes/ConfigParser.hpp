/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ConfigParser.hpp                                   :+:    :+:            */
/*                                                     +:+                    */
/*   By: totartar <totartar@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/02/19 20:08:39 by totartar      #+#    #+#                 */
/*   Updated: 2021/02/19 23:04:10 by totartar      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_PARSER_HPP
# define CONFIG_PARSER_HPP

# include <iostream>
# include <fstream>
# include <string>
# include <algorithm>
# include <queue>
# include <map>


#define BLANKS "\t\v "

enum e_context{
	MAIN,
	SERVER,
	ROUTE	
}		;


enum e_directives{
	LISTEN,
	SERVER_NAME,
	ROOT,
	ERROR_PAGE,
	LIMIT_EXCEPT,
	AUTOINDEX,
	UPLOAD_STORE,
	EMPTY,
	LAST
};

enum	e_fields{
	LEFT,
	RIGHT
};

enum	e_status {
	INVALID = -1,
	SUCCESS = 0,
	ERROR = 1
};

typedef	struct	s_routeToken
{
	s_routeToken(std::string &name) :path(name) {}
	std::map<std::string, std::string> directives;
	std::string path;//could just add path to directives, but clears confusion
}				routeToken;

typedef	struct	s_serverToken
{
	std::map<std::string, std::string> directives;
	std::queue<routeToken>	routes;
}				serverToken;


static size_t	const hash_len[] = {1, 4, 5};

static const char *blocks[] = {"server", "location",  "", 0};
static const char *main_directives[] = {0};
static const char *server_directives[] = {"listen", "server_name", "error_pages", "client_max_body_size", 0};
static const char *route_directives[] = {"index", "limit_except", "root", "autoindex", "upload_store",  0}; //add cgi later
static const char **directives_string[] = {main_directives, server_directives, route_directives};

int		addServer(std::vector<std::string>&, std::queue<serverToken> &);
int		addServerDirective(std::vector<std::string>&, std::queue<serverToken> &);
int		addRoute(std::vector<std::string>&, std::queue<serverToken> &);
int		addRouteDirective(std::vector<std::string>&, std::queue<serverToken> &);




std::string sputnbr(size_t n);

class	ConfigParser
{
	public:

	static void	parse(char const *path, std::queue<serverToken> &tokens) { 

		std::vector<std::string> fields(2);
		int	context = MAIN;
		size_t n= 0;
		std::string	line;
		std::ifstream	file(path);
		file.exceptions(std::ifstream::badbit);//will automatically throw exceptions
		while (!(file.bad() || file.fail())
				&& getline(file, line)) {
			line.erase(0, line.find_first_not_of(BLANKS, 0)); // remove blanks before first field
			line.erase(std::find(line.begin(), line.end(), '#'), line.end()); //remove comments
			if (!line.empty()) {
				sampleLine(line, fields); // get fields
				if ((context = contextParse(context, fields, tokens)) == INVALID) // call approriate parsing depending on context and update context
					throw(parseError(path, n));
			}
			n++;
		}
		if (tokens.empty())
			throw(parseError(path, "empty file"));
		file.close();
	}

	class parseError : public std::exception {
		public:
			parseError(const char *path, size_t n) 
				: _error(std::string(path) + ": syntax error line " + sputnbr(n)) {
			}
			parseError(const char *path, const char *message) 
				: _error(std::string(path) + ": " + message) {
			}
			const char *what() const throw() {
				return _error.c_str();
			}
			virtual ~parseError() throw() {}
		private:
			std::string _error;
	};

	//debug functions to print tokens after parsing
	static void	printServer(std::queue<serverToken> tokens) {
		while (!tokens.empty()) {
			serverToken	&t = tokens.front();
			std::cout << "SERVER"<<std::endl;
			for (std::map<std::string, std::string>::iterator it = t.directives.begin(); 
					it != t.directives.end(); it++) {
				std::cout<<"\t"<<it->first<<" "<<it->second<<std::endl;
			}
			printRoutes(t.routes);
			tokens.pop();
		}
	}

	static void	printRoutes(std::queue<routeToken> tokens) {
		while (!tokens.empty()) {
			routeToken &t = tokens.front();
			std::cout << "\tlocation "<<t.path<<std::endl;
			for (std::map<std::string, std::string>::iterator it = t.directives.begin(); 
					it != t.directives.end(); it++) {
				std::cout<<"\t\t"<<it->first<<" "<<it->second<<std::endl;
			}
			tokens.pop();
		}
	}

	private:
	// parse a line into left/right fields, and store them into a vector
		static void	sampleLine(std::string &line, std::vector<std::string> &fields) {
			size_t	start = line.find_first_not_of(BLANKS, 0);
			size_t	end	= line.find_first_of(BLANKS, start);
			if (line.empty())
				return ;
			fields[LEFT] = line.substr(start, end);
			if (line.size() == 1)
				fields[RIGHT].clear();
			else
				fields[RIGHT] = std::string(line.begin() + line.find_first_not_of(BLANKS, end), line.end());
		}

		//  server {
		static int		addServer(std::vector<std::string>&fields, std::queue<serverToken> &tokens) {
			if (fields[RIGHT].empty() || fields[RIGHT][0] != '{')
				return INVALID;
			tokens.push(serverToken()); // new server block
			fields[LEFT].assign(fields[RIGHT].begin() + 1, fields[RIGHT].end()); // offset parsing to rest of the line
			return SERVER; //update context
		}

		//  location /images {
		static int		addRoute(std::vector<std::string>& fields, std::queue<serverToken> &tokens) {
			size_t	bracket = fields[RIGHT].find_first_of("{", 0);
			size_t	delimiter = fields[RIGHT].find_first_of(BLANKS, 0);
			std::string		path;
			std::queue<routeToken> &routes = tokens.back().routes;
			if (bracket == std::string::npos || !bracket)
				return INVALID;
			size_t	end = delimiter > bracket ;
			if (delimiter > bracket) //only one path was found before { : /images{
				end = bracket;
			else if (fields[RIGHT].find_first_not_of(BLANKS, delimiter) == bracket) // /images {
				end = delimiter;
			if (!end)
				return INVALID;
			path.assign(fields[RIGHT].begin(), fields[RIGHT].begin() + end); //extract path
			routes.push(routeToken(path));
			/*maybe add check to see if a route with the same path already exists*/
			if (!fields[RIGHT].empty())
				fields[LEFT].assign(fields[RIGHT].begin() + bracket + 1, fields[RIGHT].end());
			return ROUTE;
		}

		static int addBlock(std::vector<std::string>& fields, std::queue<serverToken> &tokens, int context) {
			switch (context)
			{
				case MAIN : return addServer(fields, tokens);
							  break ;
				case SERVER: return addRoute(fields, tokens);
							  break ;
			}
			return INVALID;
		}


		static int addDirective(std::vector<std::string>& fields, std::queue<serverToken> &tokens, int context) {
			switch (context)
			{
				case SERVER : return addServerDirective(fields, tokens);
							  break ;
				case ROUTE : return addRouteDirective(fields, tokens);
							  break ;
			}
			return INVALID;
		}


		// get arguments until ; and push to corresponding directives
		static int addServerDirective(std::vector<std::string>& fields, std::queue<serverToken> &tokens) {
			size_t	end = fields[RIGHT].find_first_of(";", 0);
			if (end == std::string::npos)
				return INVALID;
			tokens.back().directives[fields[LEFT]] = fields[RIGHT].substr(0, end); //pushing to directives map
			if (!fields[RIGHT].empty()) //shift fields if we didn't reach eol
				fields[LEFT].assign(fields[RIGHT].begin() + end + 1, fields[RIGHT].end());
			return SUCCESS;
		}

		static int addRouteDirective(std::vector<std::string>& fields, std::queue<serverToken> &tokens) {
			size_t	end = fields[RIGHT].find_first_of(";", 0);
			if (end == std::string::npos)
				return INVALID;
			tokens.back().routes.back().directives[fields[LEFT]] = fields[RIGHT].substr(0, end);
			if (!fields[RIGHT].empty())
				fields[LEFT].assign(fields[RIGHT].begin() + end + 1, fields[RIGHT].end());
			return SUCCESS;
		}


		//for each line, checks lhs against valid directive corresponding on context
		//recursive if multiple directives in a line
		//return updated context 
		static int		contextParse(int context, std::vector<std::string> &fields, std::queue<serverToken> &tokens) {
			int		updated_context = context;
			const char **directive;

			if (fields[LEFT] == blocks[context]) {
				updated_context = addBlock(fields, tokens, context);
			} 
			else if (fields[LEFT][0] == '}') // close block
			{
				updated_context -= 1; //update to parent context
				fields[LEFT].erase(fields[LEFT].begin()); //skip bracket
				if (fields[LEFT].empty()) // shift fields if necessary
					fields[LEFT] = fields[RIGHT];
			}
			else { // check if valid directive depending on context
				directive = std::find(directives_string[context], 
											&directives_string[context][hash_len[context] - 1], 
											fields[LEFT]); //match lhs with corresponding valid strings
				if (!*directive || addDirective(fields, tokens, context) == INVALID) 
					return INVALID;
			}
			if (updated_context == INVALID 
					|| fields[LEFT].empty()) //we reached eol
				return updated_context;
			//didn't reach elo so need to resample
			//fields[LEFT] is the new line, updated in add_directive
			std::string swap = fields[LEFT];
			sampleLine(swap, fields);
			return contextParse(updated_context, fields, tokens);
		}
};
#endif
