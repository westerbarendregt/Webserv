import requests
import  os
import time
import logging

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def test_cgi_qstring_pinfo():
    print(bcolors.HEADER + bcolors.BOLD + "\ttest_cgi_qstring_pinfo" + bcolors.ENDC)
    try :
        print("POST data=query=string")
        r=requests.post('http://127.0.0.1:8080/php/check_env.php', \
                data="query=string", headers={"Content-Type": "application/x-www-form-urlencoded"})
        data=r.json()
        assert data['post_data'] == "query=string", data['post_data']
        assert data['post_parsed_query'] == "string", data['post_parsed_query']
        #---------#
        print("POST ?query=string data=query=string")
        r=requests.post('http://127.0.0.1:8080/php/check_env.php?query=string', \
                data="query=string", headers={"Content-Type": "application/x-www-form-urlencoded"})
        data=r.json()
        assert data['post_data'] == "query=string", data['post_data']
        assert data['post_parsed_query'] == "string", data['post_parsed_query']
        assert data['QUERY_STRING'] == "query=string", data['QUERY_STRING']
        #---------#
        print("POST ?query=string data=query=string&key=val")
        r=requests.post('http://127.0.0.1:8080/php/check_env.php?query=string', \
                data="query=string&key=val", headers={"Content-Type": "application/x-www-form-urlencoded"})
        data=r.json()
        assert data['post_data'] == "query=string&key=val", data['post_data']
        assert data['post_parsed_query'] == "string", data['post_parsed_query']
        assert data['post_parsed_key'] == "val", data['post_parsed_key']
        assert data['QUERY_STRING'] == "query=string", data['QUERY_STRING']
        #---------#
        print("POST /path/info/?query=string data=query=string&key=val")
        r=requests.post('http://127.0.0.1:8080/php/check_env.php/path/info?query=string', \
                data="query=string&key=val", headers={"Content-Type": "application/x-www-form-urlencoded"})
        data=r.json()
        assert data['post_data'] == "query=string&key=val", data['post_data']
        assert data['post_parsed_query'] == "string", data['post_parsed_query']
        assert data['post_parsed_key'] == "val", data['post_parsed_key']
        assert data['QUERY_STRING'] == "query=string", data['QUERY_STRING']
        assert data['PATH_INFO'] == "/php/check_env.php/path/info", data['PATH_INFO']
        #---------#
        print("GET ?query=string")
        r=requests.get('http://127.0.0.1:8080/php/check_env.php?query=string')
        data=r.json()
        assert data['QUERY_STRING'] == "query=string"
        #---------#
        print("GET /path/info")
        r=requests.get('http://127.0.0.1:8080/php/check_env.php/path/info')
        data=r.json()
        assert data['PATH_INFO'] == "/php/check_env.php/path/info"
        #---------#
        print("GET /path/info?query=string")
        r=requests.get('http://127.0.0.1:8080/php/check_env.php/path/info?query=string')
        data=r.json()
        assert data['PATH_INFO'] == "/php/check_env.php/path/info"
        assert data['QUERY_STRING'] == "query=string"
        #---------#
        print("GET /path/info/ok?query=string&key=value")
        r=requests.get('http://127.0.0.1:8080/php/check_env.php/path/info/ok?query=string&key=val')
        data=r.json()
        assert data['PATH_INFO'] == "/php/check_env.php/path/info/ok"
        assert data['QUERY_STRING'] == "query=string&key=val"
        #---------#
        print("GET index /?query=string&key=val")
        r=requests.get('http://127.0.0.1:8080/php/?query=string&key=val')
        data=r.json()
        assert data['PATH_INFO'] == "/php/check_env.php"
        assert data['QUERY_STRING'] == "query=string&key=val"
        #---------#
        print("GET index ?query=string&key=val")
        r=requests.get('http://127.0.0.1:8080/php/?query=string&key=val')
        data=r.json()
        assert data['PATH_INFO'] == "/php/check_env.php"
        assert data['QUERY_STRING'] == "query=string&key=val"
        #---------#
        print("POST index /?query=string&key=val")
        r=requests.post('http://127.0.0.1:8080/php/?query=string&key=val')
        data=r.json()
        assert data['PATH_INFO'] == "/php/check_env.php"
        assert data['QUERY_STRING'] == "query=string&key=val"
        #---------#
        print("POST index ?query=string&key=val")
        r=requests.post('http://127.0.0.1:8080/php/?query=string&key=val')
        data=r.json()
        assert data['PATH_INFO'] == "/php/check_env.php"
        assert data['QUERY_STRING'] == "query=string&key=val"

    except Exception as e:
        logging.error("FAIL", exc_info=True)
        os._exit(1);
    print(bcolors.OKGREEN+ "OK" + bcolors.ENDC)

def test_qstring_pinfo ():
    print(bcolors.HEADER + bcolors.BOLD + "\ttest_qstring_pinfo" + bcolors.ENDC)
    try:

        #----#

        print("non cgi POST /path/info/")
        r=requests.post('http://127.0.0.1:8080/default/index.html/path/info')
        assert r.status_code == 404, r.status_code
        
        print("non cgi POST ?query=string")
        r=requests.post('http://127.0.0.1:8080/default/index.html?query=string')
        assert r.status_code == 405, r.status_code
        
        print("non cgi POST /path/info?query=string")
        r=requests.post('http://127.0.0.1:8080/default/index.html/path/info?query=string')
        assert r.status_code == 404, r.status_code

        print("non cgi POST /?query=string")
        r=requests.post('http://127.0.0.1:8080/?query=string')
        assert r.status_code == 405, r.status_code

        #----#
        
        print("non cgi GET path/info")
        r=requests.get('http://127.0.0.1:8080/default/index.html/path/info')
        assert r.status_code == 404, r.status_code
        
        print("non cgi GET ?query=string")
        r=requests.get('http://127.0.0.1:8080/default/index.html?query=string')
        assert r.status_code == 200, r.status_code
        
        print("non cgi GET /path/info?query=string")
        r=requests.get('http://127.0.0.1:8080/default/index.html/path/info?query=string')
        assert r.status_code == 404, r.status_code

        print("non cgi GET /?query=string")
        r=requests.get('http://127.0.0.1:8080/?query=string')
        assert r.status_code == 200, r.status_code

        #----#
        #curl -i -X PUT "http://localhost:8080/put_test/chicken.png" --upload-file "tests/chicken.png"
        # put chicken first -> 201 created
        #curl -i -X PUT "http://localhost:8080/put_test/chicken.png" --upload-file "tests/chicken.png"
        # put chicken second -> 204 no content

        # PUT query=string
        #lurl -i -X PUT "http://localhost:8080/put_test/chicken.png?query=string" --upload-file "tests/chicken.png"
        # put chicken first -> 201 created
        #lurl -i -X PUT "http://localhost:8080/put_test/chicken.png?query=string" --upload-file "tests/chicken.png"
        # put chicken second -> 204 no content

        # PUT /path/info
        #curl -i -X PUT "http://localhost:8080/put_test/chicken.png/path/info" --upload-file "tests/chicken.png"
        # put chicken first -> 500
        #curl -i -X PUT "http://localhost:8080/put_test/chicken.png/path/info" --upload-file "tests/chicken.png"
        # put chicken second -> 500
        #500

        # PUT /path/info?query=string
        #curl -i -X PUT "http://localhost:8080/put_test/chicken.png/path/info?query=string" --upload-file "tests/chicken.png"
        # put chicken first -> 500
        #curl -i -X PUT "http://localhost:8080/put_test/chicken.png/path/info?query=string" --upload-file "tests/chicken.png"
        # put chicken second -> 500
        #500

    except Exception as e:
        logging.error(bcolors.FAIL + bcolors.BOLD + "FAIL" + bcolors.ENDC, exc_info=True)
        os._exit(1);
    print(bcolors.OKGREEN+ "OK" + bcolors.ENDC)

test_cgi_qstring_pinfo()
test_qstring_pinfo()
