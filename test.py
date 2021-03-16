import requests
import  os
import time

#test a request with different ip:port and host header combination
#r=requests.get('http://127.0.0.1:8080/index.html/', headers={"Host": "no"})
#r=requests.get('http://127.0.0.1:8080/path/to/index.html', headers={"Host": "no"})



#r=requests.get('http://127.0.0.1:8080/path/to/test-cgi.php', headers={"Host": "no"})
#r=requests.get('http://127.0.0.1:8080/path/to/test-cgi.php/path/info', headers={"Host": "no"})

#r=requests.get('http://127.0.0.1:8080/path/to/test-cgi.php/path/info?query=string', headers={"Host": "no"})

#r=requests.get('http://127.0.0.1:8080/path/to/test-cgi.php?query=string', headers={"Host": "no"})

#r=requests.get('http://127.0.0.1:8080/ok/index.html', headers={"Host": "no"})
#
#
#print(r.text)


def test_async_cgi():
    print("sending cgi1 request")
    cgi_pid = os.fork()
    if cgi_pid == 0:
        try:
            r=requests.get('http://127.0.0.1:8080/path/to/test-cgi.php', headers={"Host": "no"})
        except Exception:
            pass
        os._exit(0)
    time.sleep(1)
    print("sending cgi2 request")
    cgi_pid2 = os.fork()
    if cgi_pid2 == 0:
        try:
            r=requests.get('http://127.0.0.1:8080/path/to/test-cgi.php', headers={"Host": "no"})
        except Exception:
            pass
        os._exit(0)
    time.sleep(2)
    print("other request while cgi is processing")
    try:
        r=requests.get('http://127.0.0.1:8080/path/to/index.html', headers={"Host": "no"})
    except Exception:
        pass
    print("received other request")
    status = os.waitpid(cgi_pid, 0)
    print("received cgi1")
    status = os.waitpid(cgi_pid2, 0)
    print("received cgi2")

test_async_cgi()
