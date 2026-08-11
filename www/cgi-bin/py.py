#!/usr/bin/python3

# Import CGI and CGIT module

# to create instance of FieldStorage 
# class which we can use to work 
# with the submitted form data
SERVER_NAME = "hello"


print ("Content-type:text/html\n")
print ("<html>")
print ("<head>")
print ("<title>First CGI Program</title>")
print ("</head>")
print ("<body>")
print(SERVER_NAME);

print ("</body>")
print ("</html>")
