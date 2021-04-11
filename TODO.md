- [ ] Adding the value in the index directive is not compatible with query strings / cgi
- [x] On PUT requests check file permissions before calling open
- [x] test what nginx does when it doesn't have permissions
- [x] Make compare function in python script that compares nginx with our webserv
- [x] Add signal handler for sigterm / ctrl-c
- [x] Test with chunked input
- [?] Implement missing headers
- [ ] Accept-Charset
- [ ] User-Agent
- [ ] Referer
- [ ] Retry-After
- [ ] Write tests for POST cgi with html forms
- [ ] post really always 200


Monday 29-03
- [x] Fix getLocation with alias/
- [x] write uri parser test
- [x] Make comparing python tester work with a result of 1 or 0
- READ THE PAPER!
-  Implement some checks with hostname:port
	- [x] for host headers that include a port, only compare what precedes the port
	- [ ] write tests for multi_name.conf
	- [x] handle duplicate server_name for same port
- [x] try some tests with siege 
- [x] Running the tester
- [x] fix test GET Expected 404 on http://localhost:8080/directory/Yeah
- [x] add variable MACOS/LINUX portability for tester variable
- [ ] we are allowed to use unlink, maybe apply it at DELETE if you're bored or the paper says so... 
- [ ] CGI implementing the x_headers!, there in a map in the struct request
- [ ] check the buffer recv size, might be high
- [ ] Add log levels for the output -log [cerr-cout]
- [ ] check how exactly tv influences select()
- [x] handle put trim upload_store better
- [ ] write some more test for compare python



!!
