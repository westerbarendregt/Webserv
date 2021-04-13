import requests
import  os
import time

def test_async_cgi_get():
    print("\t\ttest_async_cgi_get")
    try :
        print("sending cgi1 request")
        cgi_pid = os.fork()
        if cgi_pid == 0:
            r=requests.get('http://127.0.0.1:8080/path/to/simple_form.php', headers={"Host": "no"})
            os._exit(0)
        time.sleep(1)
        print("sending cgi2 request")
        cgi_pid2 = os.fork()
        if cgi_pid2 == 0:
            r=requests.get('http://127.0.0.1:8080/path/to/simple_form.php', headers={"Host": "no"})
            os._exit(0)
        time.sleep(2)
        print("other request while cgi is processing")
        r=requests.get('http://127.0.0.1:8080/path/to/index.html', headers={"Host": "no"})
        print("received other request")
        status = os.waitpid(cgi_pid, 0)
        print("received cgi1")
        status = os.waitpid(cgi_pid2, 0)
        print("received cgi2")
    except Exception as e :
        print(e)
        print("---FAILED---")
        os._exit(1)
    print("---OK---")

def test_async_cgi_post():
    print("\t\ttest_async_cgi_post")
    try :
        print("sending cgi1 request")
        cgi_pid = os.fork()
        if cgi_pid == 0:
            r=requests.post('http://127.0.0.1:8080/path/to/simple_form.php', \
                    data="query=string", headers={"Content-Type": "application/x-www-form-urlencoded"})
            os._exit(0)
        time.sleep(1)
        print("sending cgi2 request")
        cgi_pid2 = os.fork()
        if cgi_pid2 == 0:
            r=requests.post('http://127.0.0.1:8080/path/to/simple_form.php', \
                    data="user=HANS", headers={"Content-Type": "application/x-www-form-urlencoded"})
            os._exit(0)
        time.sleep(2)
        print("other request while cgi is processing")
        r=requests.get('http://127.0.0.1:8080/path/to/index.html', headers={"Host": "no"})
        print("received other request")
        status = os.waitpid(cgi_pid, 0)
        print("received cgi1")
        status = os.waitpid(cgi_pid2, 0)
        print("received cgi2")
    except Exception as e:
        print(e)
        print("---FAILED---")
        os._exit(1);
    print("---OK---")

def test_cgi():
    try :
        print("-------POST data=query=string")
        r=requests.post('http://127.0.0.1:8080/php/post_test_raw.php', \
                data="query=string", headers={"Content-Type": "application/x-www-form-urlencoded"})
        print(r.text)
        #---------#
        print("------POST ?query=string data=query=string")
        r=requests.post('http://127.0.0.1:8080/php/post_test_raw.php?query=string', \
                data="query=string", headers={"Content-Type": "application/x-www-form-urlencoded"})
        print(r.text)
        #---------#
        print("--------POST ?query=string data=query=string&key=val")
        r=requests.post('http://127.0.0.1:8080/php/post_test_raw.php?query=string', \
                data="query=string&key=val", headers={"Content-Type": "application/x-www-form-urlencoded"})
        print(r.text)
        #---------#
        print("--------POST /path/info/?query=string data=query=string&key=val")
        r=requests.post('http://127.0.0.1:8080/php/post_test_raw.php/path/info?query=string', \
                data="query=string&key=val", headers={"Content-Type": "application/x-www-form-urlencoded"})
        print(r.text)
        #---------#
        print("--------GET ?query=string\n")
        r=requests.get('http://127.0.0.1:8080/php/post_test_raw.php?query=string')
        print(r.text)
        #---------#
        print("--------GET /path/info\n")
        r=requests.get('http://127.0.0.1:8080/php/post_test_raw.php/path/info')
        print(r.text)
        #---------#
        print("--------GET /path/info?query=string\n")
        r=requests.get('http://127.0.0.1:8080/php/post_test_raw.php/path/info?query=string')
        print(r.text)
        #---------#
        print("--------GET /path/info/ok?query=string&key=value\n")
        r=requests.get('http://127.0.0.1:8080/php/post_test_raw.php/path/info/ok?query=string&key=val')
        print(r.text)

    except Exception as e:
        print(e)
        print("---FAILED---")
        os._exit(1);
    print("---OK---")

def test_qstring_pinfo ():
    try:
        print("-------- non cgi POST /path/info/\n")
        r=requests.post('http://127.0.0.1:8080/default/index.html/path/info')
        assert r.status_code == 404
        
        print("--------non cgi POST ?query=string\n")
        r=requests.post('http://127.0.0.1:8080/default/index.html?query=string')
        assert r.status_code == 405
        
        print("-------- non cgi POST /path/info?query=string\n")
        r=requests.post('http://127.0.0.1:8080/default/index.html/path/info?query=string')
        assert r.status_code == 404
        
        print("--------non cgi GET path/info\n")
        r=requests.get('http://127.0.0.1:8080/default/index.html/path/info')
        assert r.status_code == 404
        
        print("--------non cgi GET ?query=string\n")
        r=requests.get('http://127.0.0.1:8080/default/index.html?query=string')
        assert r.status_code == 200
        
        print("--------non cgi GET /path/info?query=string\n")
        r=requests.get('http://127.0.0.1:8080/default/index.html/path/info?query=string')
        assert r.status_code == 404
    except Exception as e:
        print(e)
        print("---FAILED---")
        os._exit(1);
    print("---OK---")

#test_async_cgi_get()
#test_async_cgi_post()
#test_cgi()
test_qstring_pinfo()
