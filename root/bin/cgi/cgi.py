#!/usr/bin/python3

# Import necessary library for CGI handling
import cgi

# Set the content type to HTML
print("Content-type: text/html\n\n")

# Print the HTML content
print("<html>")
print("<head><title>Hello World CGI</title></head>")
print("<body>")
print("<h1>Hello, World! This is a Python CGI script!</h1>")
print("</body>")
print("</html>")