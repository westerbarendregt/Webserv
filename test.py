import requests

#test a request with different ip:port and host header combination
r=requests.get('http://127.0.0.1:8080/path/to/images/ok/non', headers={"Host": "no"})
print(r.text)
