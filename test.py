import requests

#test a request with different ip:port and host header combination
#r=requests.get('http://127.0.0.1:8080/index.html/', headers={"Host": "no"})
#r=requests.get('http://127.0.0.1:8080/index.html', headers={"Host": "no"})
#r=requests.get('http://127.0.0.1:8080/index.html/ok', headers={"Host": "no"})
#r=requests.get('http://127.0.0.1:8080/index.html/ok/', headers={"Host": "no"})
#r=requests.get('http://127.0.0.1:8080/index.html/ok/non', headers={"Host": "no"})
#r=requests.get('http://127.0.0.1:8080/test-cgi.php/ok/non/', headers={"Host": "no"})
#r=requests.get('http://127.0.0.1:8080/test-cgi.php/ok/non?oui=oui&non=non', headers={"Host": "no"})
r=requests.get('http://127.0.0.1:8080/test-cgi.php', headers={"Host": "no"})
#r=requests.get('http://127.0.0.1:8080/index.html/ok/non?oui=oui&non=non', headers={"Host": "no"})
#r=requests.get('http://127.0.0.1:8080/index.html/', headers={"Host": "no"})
#r=requests.get('http://127.0.0.1:8080/hey/', headers={"Host": "no"})
#print(r.text)
