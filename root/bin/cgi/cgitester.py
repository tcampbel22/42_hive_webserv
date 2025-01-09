#!/usr/bin/env python3

# Import necessary library for CGI handling
# import cgi

# import os
# from urllib.parse import parse_qs, unquote

# def parse_query_strings(query_string):
#  decoded_string = unquote(query_string) # Decodes the URL-encoded query string (e.g., converts '%20' to a space)
#  parsed_key_value_pairs = parse_qs(decoded_string) # Parses the query string into key-value pairs (as a dictionary)
#  params = {key: value[0] for key, value in parsed_key_value_pairs.items()} #makes dictionary which is returned, moves the list wrapper from parsed_key_value_pairs 

#  return params

# def handle_get():
#  query_string = os.getenv("QUERY_STRING")

#  if query_string:
#   query_pairs = parse_query_strings(query_string)
#   print("<html>")
#   print("<body>")
#   print("<h1>Received GET Request</h1>")
#   print("<p>Parsed Parameters:</p>")
#   print("<ul>")
#   for key, value in query_pairs.items():
#     print(f"<li>{key}: {value}</li>")
#   print("</ul>")
#   print("</body>")
#   print("</html>")
#   print()
#  else:
#     print("<html><body><h1>Whats up this is the cgi</h1></body></html>")
#     print()
#  print("<a href=/index.html><button>Front-Page</button></a>")

# def upload_file():
# if not os.path.exists(UPLOAD_DIR):
#     os.makedirs(UPLOAD_DIR)

# # Get the form data
#  form = cgi.FieldStorage()

# # Check if the file was uploaded
#  file_item = form['file']

#  if file_item.filename:
#     # Sanitize the filename
#     filename = os.path.basename(file_item.filename)
#     filepath = os.path.join(UPLOAD_DIR, filename)

#     # Open a new file to write the uploaded content
#     with open(filepath, 'wb') as f:
#         # Copy the file content to the new file
#         f.write(file_item.file.read())

#     # Send a success message
#     print("Content-Type: text/html\n")
#     print("<html><body>")
#     print(f"<h2>File uploaded successfully!</h2>")
#     print(f"<p>Your file has been saved as {filename}.</p>")
#     print("</body></html>")

# def handle_post():
# # Create FieldStorage manually using the read data
#  contentType = os.getenv("CONTENT_TYPE")
#  if contentType.find("multipart/form-data") != -1:
#         upload_file()
# 		return:
#  form = cgi.FieldStorage()
#  if form:
# 		print("<html><body>")
# 		print("<h1>Parsed Post</h1>")
# 		print("<ul>")
# 		for field in form.keys():
# 			field_value = form.getvalue(field)
# 			print(f"<li>{field}: {field_value}</li>")
# 		print("</ul></body></html>")
# 		print()
# 		print("<a href=/checkbox.html><button>Back</button></a>")
# 		print()
# 		print("<a href=/index.html><button>Front-Page</button></a>")
#  else:
# 		print("<html><body><h1>No POST data received</h1></body></html>")

# def main():
#     # Get the request method (GET or POST)
#     request_method = os.getenv("REQUEST_METHOD")

#     if request_method == "GET":
#         handle_get()  # Handle GET request
#     elif request_method == "POST":
#         handle_post()  # Handle POST request
#     else:
#         print("Content-Type: text/html")
#         print()  # Blank line separates headers from body
#         print("<html><body><h1>Unsupported request method</h1></body></html>")

# if __name__ == "__main__":
#     main()

# Import necessary libraries
import cgi
import os
from urllib.parse import parse_qs, unquote

UPLOAD_DIR = './root/var/www/temp'  # Define your upload directory

def parse_query_strings(query_string):
    decoded_string = unquote(query_string)  # Decodes the URL-encoded query string
    parsed_key_value_pairs = parse_qs(decoded_string)  # Parse into key-value pairs
    params = {key: value[0] for key, value in parsed_key_value_pairs.items()}  # Flatten the dictionary
    return params

def handle_get():
    query_string = os.getenv("QUERY_STRING")
    print("Content-Type: text/html\n")  # Make sure to output headers first
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

def upload_file():
    if not os.path.exists(UPLOAD_DIR):
        os.makedirs(UPLOAD_DIR)  # Ensure the upload directory exists

    # Get the form data
    form = cgi.FieldStorage()

    # Check if the file was uploaded
    file_item = form['file']
    if file_item.filename:
        # Sanitize the filename
        filename = os.path.basename(file_item.filename)
        filepath = os.path.join(UPLOAD_DIR, filename)

        # Open a new file to write the uploaded content
        with open(filepath, 'wb') as f:
            f.write(file_item.file.read())

        # Send a success message
        print("Content-Type: text/html\n")
        print("<html><body>")
        print(f"<h2>File uploaded successfully!</h2>")
        print(f"<p>Your file has been saved as {filename}.</p>")
        print("</body></html>")
        return  # Stop further processing

def handle_post():
    # Create FieldStorage manually using the read data
    contentType = os.getenv("CONTENT_TYPE")
    print("Content-Type: text/html\n")
    
    if contentType and "multipart/form-data" in contentType:
        upload_file()  # Handle file upload
        return  # Stop further processing after file upload

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

    if request_method == "GET":
        handle_get()  # Handle GET request
    elif request_method == "POST":
        handle_post()  # Handle POST request
    else:
        print("Content-Type: text/html\n")
        print("<html><body><h1>Unsupported request method</h1></body></html>")

if __name__ == "__main__":
    main()