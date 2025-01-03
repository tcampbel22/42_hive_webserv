#!/usr/bin/env python3

# Import necessary library for CGI handling
import cgi

import os
from urllib.parse import parse_qs, unquote

def parse_query_strings(query_string):
 decoded_string = unquote(query_string)
 parsed_key_value_pairs = parse_qs(decoded_string)
 params = {key: value[0] for key, value in parsed_key_value_pairs.items()}

 return params

def handle_get():
 query_string = os.getenv("QUERY_STRING")
 print("Content-Type: text/html")
#  print("Content-Type:" os.getenv("CONTENT_TYPE"))
#  print(os.getenv("SERVER_NAME"))
#  print(os.getenv("SERVER_PORT"))
#  print(os.getenv("REMOTE_ADDR"))
 print()

 if query_string:
  query_pairs = parse_query_strings(query_string)
  print("<html>")
  print("<body>")
  print("<h1>Received GET Request</h1>")
  print("<p>Parsed Parameters:</p>")
  print("<ul>")
  for key, value in query_pairs.items():
    print(f"<li>{key}: {value}</li>")
  print("</ul>")
  print("</body>")
  print("</html>")
 else:
        print("<html><body><h1>No query string received</h1></body></html>")

def handle_post():
# Create FieldStorage manually using the read data
 form = cgi.FieldStorage()

#  print(os.getenv("CONTENT_TYPE"))
#  print(os.getenv("SERVER_NAME"))
#  print(os.getenv("SERVER_PORT"))
#  print()

 if form:
		print("HTTP/1.1 200 OK")
		print("Content-Type: text/html")
		print("Content-Length: " + os.getenv("CONTENT_LENGTH"))	
		print()
		print("<html><body>")
		#print("<p>Parsed Parameters:</p>")
		print("<ul>")
		for field in form.keys():
			field_value = form.getvalue(field)
			print(f"<li>{field}: {field_value}</li>")
		print("</ul></body></html>")
 else:
		print("<html><body><h1>No POST data received</h1></body></html>")

def main():
    # Get the request method (GET or POST)
    request_method = os.getenv("REQUEST_METHOD")

    if request_method == "GET":
        handle_get()  # Handle GET request
    elif request_method == "POST":
        handle_post()  # Handle POST request
    else:
        print("Content-Type: text/html")
        print()  # Blank line separates headers from body
        print("<html><body><h1>Unsupported request method</h1></body></html>")

if __name__ == "__main__":
    main()