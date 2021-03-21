import requests
import  os
import time

def test_async_cgi_get():
    print("sending cgi1 request")
    cgi_pid = os.fork()
    if cgi_pid == 0:
        try:
            r=requests.get('http://127.0.0.1:8080/path/to/simple_form.php', headers={"Host": "no"})
        except Exception:
            pass
        os._exit(0)
    time.sleep(1)
    print("sending cgi2 request")
    cgi_pid2 = os.fork()
    if cgi_pid2 == 0:
        try:
            r=requests.get('http://127.0.0.1:8080/path/to/simple_form.php', headers={"Host": "no"})
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

def test_async_cgi_post():
    print("test_async_cgi_post")
    print("sending cgi1 request")
    cgi_pid = os.fork()
    if cgi_pid == 0:
        try:
            r=requests.post('http://127.0.0.1:8080/path/to/simple_form.php', \
                    data="query=string", headers={"Content-Type": "application/x-www-form-urlencoded"})
        except Exception:
            pass
        os._exit(0)
    time.sleep(1)
    print("sending cgi2 request")
    cgi_pid2 = os.fork()
    if cgi_pid2 == 0:
        try:
            r=requests.post('http://127.0.0.1:8080/path/to/simple_form.php', \
                    data="user=HANS", headers={"Content-Type": "application/x-www-form-urlencoded"})
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


def test_cgi_post():
    r=requests.post('http://127.0.0.1:8080/path/to/simple_form.php', \
            data="query=string", headers={"Content-Type": "application/x-www-form-urlencoded"})

#test_async_cgi_get()
#test_async_cgi_post()
#test_cgi_post()

