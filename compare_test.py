#!/usr/bin/python

import sys,subprocess,os

webservPort = "5000"
nginxPort= "8080"
webservFile = open("webservFile", "r+")
webservFile.truncate(0)
nginxFile = open("nginxFile", "r+")
nginxFile.truncate(0)

def runPort(port, file, method, headers, body, url_file_name):
    # url_file_name = "/put_test/101" + port + ".txt"
    if method == "PUT":
        url_file_name += port
    host = "localhost:"+port+url_file_name
    # method = "PUT"
    # auth = "'AUTHORIZATION: Basic d2Vic2VydjpjaGVlc2U='"
    file_keys = "\"sleutel=hallo webserver\""
    # print (host)
    args = []
    args.append("curl")
    args.append("-i")
    args.append("-X")
    args.append(method)
    args.append("-H")
    # args.append(auth)
    for i in headers:
        args.append(i)
    args.append("-F")
    args.append(file_keys)
    args.append(host)
    output = subprocess.run(args, capture_output=True)
    # print (output.stdout.decode())
    file.write(output.stdout.decode())
    file.write("-----------------------------------------------"+port+"\n")
    # file.close()
    # print("hello")


def runCommand(port):
    runBoth("PUT", ["'AUTHORIZATION: Basic d2Vic2VydjpjaGVlc2U='", "Connection: keep-alive"], "", 
            "/put_test/111.txt", port)
    runBoth("POST", "", "hello", "/directory/", port)
    nginxFile.close()
    webservFile.close()
    os.system("diff webservFile nginxFile")

def runBoth(method, headers, body, url, port):#webservPort, nginxPort):
    if port == 0:
        runPort(webservPort, webservFile, method, headers, body, url)
        runPort(nginxPort, nginxFile, method, headers, body, url)
    elif port == "8080":
        runPort(port, nginxFile, method, headers, body, url)
    elif port == "5000":
        runPort(port, webservFile, method, headers, body, url)



print ("Make sure webserv is running on", webservPort, "and nginx on", nginxPort)

if len(sys.argv) != 2:
    print ('Please give 1 argument')
    exit()

if sys.argv[1] == 'nginx':
    runCommand(nginxPort)
elif sys.argv[1] == 'webserv':
    runCommand(webservPort)
elif sys.argv[1] == 'compare':
    runCommand(0)#webservPort, nginxPort)
else :
    print ("NOT RECOGNIZED - first argument should be: nginx, webserv or compare")
    
