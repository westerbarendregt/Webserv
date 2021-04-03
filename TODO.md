- [ ] Adding the value in the index directive is not compatible with query strings / cgi
- [x] On PUT requests check file permissions before calling open
- [x] test what nginx does when it doesn't have permissions
- [x] Make compare function in python script that compares nginx with our webserv
- [ ] Add signal handler for sigterm / ctrl-c
- [ ] Test with chunked input
- [ ] Implement missing headers
- [ ] Write tests for POST cgi with html forms


Monday 29-03
- [x] Fix getLocation with alias/
- [ ] write uri parser test
- [ ] Make comparing python tester work with a result of 1 or 0
- READ THE PAPER!
-  Implement some checks with hostname:port
	- [x] for host headers that include a port, only compare what precedes the port
	- [ ] write tests for multi_name.conf
	- [ ] handle duplicate server_name for same port
- [ ] try some tests with siege 
- [ ] Running the tester
- [ ] Handling POST better on none CGI requests
- [ ] fix test GET Expected 404 on http://localhost:8080/directory/Yeah
- [ ] add variable MACOS/LINUX portability for tester variable
