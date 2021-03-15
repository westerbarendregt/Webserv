import requests

#test a request with different ip:port and host header combination
#r=requests.get('http://127.0.0.1:8080/index.html/', headers={"Host": "no"})



#r=requests.get('http://127.0.0.1:8080/path/to/test-cgi.php', headers={"Host": "no"})
#r=requests.get('http://127.0.0.1:8080/path/to/test-cgi.php/path/info', headers={"Host": "no"})

#r=requests.get('http://127.0.0.1:8080/path/to/test-cgi.php/path/info?query=string', headers={"Host": "no"})

#r=requests.get('http://127.0.0.1:8080/path/to/test-cgi.php?query=string', headers={"Host": "no"})

#r=requests.get('http://127.0.0.1:8080/path/to/dir/', headers={"Host": "no"})

#print(r.text)
