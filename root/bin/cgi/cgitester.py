#!/usr/bin/env python3

import cgi
import os
from urllib.parse import parse_qs, unquote

def parse_query_strings(query_string):
    decoded_string = unquote(query_string)  # Decodes the URL-encoded query string
    parsed_key_value_pairs = parse_qs(decoded_string)  # Parse into key-value pairs
    params = {key: value[0] for key, value in parsed_key_value_pairs.items()}  # Flatten the dictionary
    return params

def handle_get():
    query_string = os.getenv("QUERY_STRING")
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
        print("<html><body><h1>Whats up this is the cgi</h1></body></html>")
    print("<a href='/index.html'><button>Front-Page</button></a>")

def handle_post():
    # Create FieldStorage manually using the read data
    form = cgi.FieldStorage()
    if form:
        print("<html><body>")
        print("<h1>Parsed POST Data</h1>")
        print("<ul>")
        for field in form.keys():
            field_value = form.getvalue(field)
            print(f"<li>{field}: {field_value}</li>")
        print("</ul></body></html>")
        print("<a href='/checkbox.html'><button>Back</button></a>")
        print("<a href='/index.html'><button>Front-Page</button></a>")
    else:
        print("<html><body><h1>No POST data received</h1></body></html>")

def main():
    # Get the request method (GET or POST)
    request_method = os.getenv("REQUEST_METHOD")
    # i = 0
    # while (True):
    #     i+=1
    if request_method == "GET":
        handle_get()  # Handle GET request
    elif request_method == "POST":
        handle_post()  # Handle POST request
    else:
        print("Content-Type: text/html\n")
        print("<html><body><h1>Unsupported request method</h1></body></html>")

if __name__ == "__main__":
    main()