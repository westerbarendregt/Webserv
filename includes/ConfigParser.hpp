#ifndef CONFIG_PARSER_HPP
# define CONFIG_PARSER_HPP

# include <iostream>
# include <fstream>
# include <string>
# include <algorithm>
# include <queue>
# include <map>
# include "Server.hpp"
# include "Conf.hpp"
# include "WebServer.hpp"
# include "utils.hpp"

#define BLANKS "\t\v "

enum e_context{
	MAIN_CONTEXT,
	SERVER_CONTEXT,
	ROUTE_CONTEXT	
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

static size_t	const hash_len[] = {1, 4, 5};

static const char *blocks[] = {"server", "location"};
static const char *main_directives[] = {0};
static const char *server_directives[] = {"listen", "server_name", "error_pages", "client_max_body_size", 0};
static const char *route_directives[] = {"index", "limit_except", "root", "autoindex", "upload_store",  0}; //add cgi later
static const char **directives_string[] = {main_directives, server_directives, route_directives};

std::string sputnbr(size_t n);


class	ConfigParser
{
	public:
		typedef Server::t_v_server_conf		t_v_server_conf;
		typedef Server::t_v_server			t_v_server;
		typedef Server::t_v_server_blocks			t_v_server_blocks;
		typedef Server::t_v_server_all		t_v_server_all;
		typedef Server::t_ip_port			t_ip_port;
		typedef	std::queue<t_v_server_conf>	t_config_tokens;
		typedef	std::vector<std::string>			t_fields;


	static void	formatIp(std::string &listen) {
		size_t	end = listen.find_first_of(":", 0);
		if (end == std::string::npos) {
			listen.insert(0, "0.0.0.0:");
		}
	}

	static	void	convertTokens(t_config_tokens &tokens, t_v_server_all &v_server_all) {
		t_v_server_conf	conf;
		while (!tokens.empty()) {
			conf = tokens.front();
			std::string &listen = conf.m_directives["listen"];
			formatIp(listen);
			//add check for invalid ip:port
			v_server_all[listen].push_back(t_v_server(conf));
			tokens.pop();
		}
	}

	static void	parse(char const *path, t_v_server_all &v_server_all) { 

		t_fields fields(2);
		t_config_tokens	tokens;
		int	context = MAIN_CONTEXT;
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
		convertTokens(tokens, v_server_all);
		printBlocks(v_server_all);
	}

	class parseError : public std::exception {
		public:
			parseError(const char *path, size_t n) 
				: _error(std::string(path) + ": syntax error line " + sputnbr(n) + "\n") {
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

	static void	printBlocks(t_v_server_all &v_server_all) {
			for (std::map<t_ip_port, t_v_server_blocks>::iterator ip_port = v_server_all.begin()
					; ip_port != v_server_all.end(); ++ip_port){

				std::cout << "\t\t---"<<ip_port->first<<"---"<<std::endl;
				for (size_t	i = 0; i < ip_port->second.size(); ++i)
				{
					t_v_server_conf &c = ip_port->second[i].m_configs;
					std::cout << "\t\tSERVER"<<std::endl;
					for (t_v_server_conf::t_directives::iterator it = c.m_directives.begin();
							it != c.m_directives.end(); ++it) {
						std::cout<<"\t\t\t"<<it->first<<" "<<it->second<<std::endl;
					}
					for (t_v_server_conf::t_routes::iterator path = c.m_routes.begin();
							path != c.m_routes.end(); path++) {
						std::cout << "\t\t\tROUTE "<<path->first<<std::endl;
						for (t_v_server_conf::t_directives::iterator path_directives = path->second.begin()
								; path_directives != path->second.end();
								++path_directives)
							std::cout<<"\t\t\t\t"<<path_directives->first<<" "<<path_directives->second<<std::endl;
					}
				}
		}
	}
	//debug functions to print tokens after parsing
	static void	printServerTokens(t_config_tokens tokens) {
		while (!tokens.empty()) {
			t_v_server_conf &t = tokens.front();
			std::cout << "SERVER"<<std::endl;
			for (t_v_server_conf::t_directives::iterator it = t.m_directives.begin(); 
					it != t.m_directives.end(); ++it) {
				std::cout<<"\t"<<it->first<<" "<<it->second<<std::endl;
			}
			for (t_v_server_conf::t_routes::iterator path = t.m_routes.begin(); 
					path != t.m_routes.end(); path++) {
				std::cout << "\tROUTE "<<path->first<<std::endl;
				for (t_v_server_conf::t_directives::iterator path_directives = path->second.begin()
						; path_directives != path->second.end();
						++path_directives)
					std::cout<<"\t\t"<<path_directives->first<<" "<<path_directives->second<<std::endl;
			}
			tokens.pop();
		}
	}


	private:
	// parse a line into left/right fields, and store them into a vector
		static void	sampleLine(std::string &line, t_fields &fields) {
			if (line.empty())
				return ;
			size_t	start = line.find_first_not_of(BLANKS, 0);
			if (start == std::string::npos)
			{
				fields[LEFT].clear();
				fields[RIGHT].clear();
				return ;
			}
			size_t	end	= line.find_first_of(BLANKS, start);
			fields[LEFT] = line.substr(start, end);
			if (line.size() == 1)
				fields[RIGHT].clear();
			else
				fields[RIGHT] = std::string(line.begin() + line.find_first_not_of(BLANKS, end), line.end());
		}

		//  server {
		static int		addServer(t_fields &fields, t_config_tokens &tokens) {
			if (fields[RIGHT].empty() || fields[RIGHT][0] != '{')
				return INVALID;
			tokens.push(t_v_server_conf()); // new server block
			fields[LEFT].assign(fields[RIGHT].begin() + 1, fields[RIGHT].end()); // offset parsing to rest of the line
			return SERVER_CONTEXT; //update context
		}

		//  location /images {
		static int		addRoute(t_fields &fields, t_config_tokens &tokens) {
			size_t	bracket = fields[RIGHT].find_first_of("{", 0);
			size_t	delimiter = fields[RIGHT].find_first_of(BLANKS, 0);
			std::string		path;
			//std::map<std::string, std::map<std::string, std::string> > &parsedroutes = tokens.back().m_routes;
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
			t_v_server_conf	&current_server = tokens.back();
			current_server.m_routes[path] = t_v_server_conf::t_directives();
			current_server.m_route_indexes.push_back(path);
			/*maybe add check to see if a route with the same path already exists*/
			if (!fields[RIGHT].empty())
				fields[LEFT].assign(fields[RIGHT].begin() + bracket + 1, fields[RIGHT].end());
			return ROUTE_CONTEXT;
		}

		static int addBlock(t_fields &fields, t_config_tokens &tokens, int context) {
			switch (context)
			{
				case MAIN_CONTEXT : return addServer(fields, tokens);
				case SERVER_CONTEXT: return addRoute(fields, tokens);
			}
			return INVALID;
		}


		static int addDirective(t_fields &fields, t_config_tokens &tokens, int context) {
			switch (context)
			{
				case SERVER_CONTEXT : return addServerDirective(fields, tokens);
				case ROUTE_CONTEXT : return addRouteDirective(fields, tokens);
			}
			return INVALID;
		}


		// get arguments until ; and push to corresponding directives
		static int addServerDirective(t_fields &fields, t_config_tokens &tokens) {
			size_t	end = fields[RIGHT].find_first_of(";", 0);
			if (end == std::string::npos)
				return INVALID;
			tokens.back().m_directives[fields[LEFT]] = fields[RIGHT].substr(0, end); //pushing to directives map
			if (!fields[RIGHT].empty()) //shift fields if we didn't reach eol
				fields[LEFT].assign(fields[RIGHT].begin() + end + 1, fields[RIGHT].end());
			return SUCCESS;
		}

		static int addRouteDirective(t_fields &fields, t_config_tokens &tokens) {
			size_t	end = fields[RIGHT].find_first_of(";", 0);
			if (end == std::string::npos)
				return INVALID;
			t_v_server_conf& current_server = tokens.back();
			std::string &current_route = current_server.m_route_indexes.back();
			current_server.m_routes[current_route][fields[LEFT]] = fields[RIGHT].substr(0, end);
			if (!fields[RIGHT].empty())
				fields[LEFT].assign(fields[RIGHT].begin() + end + 1, fields[RIGHT].end());
			return SUCCESS;
		}
		//for each line, checks lhs against valid directive corresponding on context
		//recursive if multiple directives in a line
		//return updated context 
		static int		contextParse(int context, std::vector<std::string> &fields, std::queue<t_v_server_conf> &tokens) {
			int		updated_context = context;
			const char **directive;

			if (fields[LEFT].empty())
				return context;
			if (context < ROUTE_CONTEXT && fields[LEFT] == blocks[context]) {
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
