import requests

#test a request with different ip:port and host header combination
r=requests.get('http://127.0.0.3:5000/', headers={"Host": "no"})
print(r.text)
