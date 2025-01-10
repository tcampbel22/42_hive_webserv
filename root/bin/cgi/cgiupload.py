#!/usr/bin/env python3

# import cgi
# import os
# from urllib.parse import parse_qs, unquote

# UPLOAD_DIR = './root/var/www/temp'

# def upload_file():
#     if not os.path.exists(UPLOAD_DIR):
#         os.makedirs(UPLOAD_DIR)  # Ensure the upload directory exists

#     # Get the form data
#     form = cgi.FieldStorage()

#     # Check if the file was uploaded
#     for field_name in form.keys()
#      file_item = form[field_name]
#     if file_item.filename:
#         # Sanitize the filename
#         filename = os.path.basename(file_item.filename)
#         filepath = os.path.join(UPLOAD_DIR, filename)

#         # Open a new file to write the uploaded content
#         with open(filepath, 'wb') as f:
#             f.write(file_item.file.read())


#         # Send a success message
#         print("Content-Type: text/html\n")
#         print("<html><body>")
#         print(f"<h2>File uploaded successfully!</h2>")
#         print(f"<p>Your file has been saved as {filename}.</p>")
#         print("</body></html>")
#         return  # Stop further processing
# 	print("Content-Type: text/html\n")
#     print("<html><body><h1>No file uploaded</h1></body></html>")

# def main():

# request_method = os.getenv("REQUEST_METHOD")
# contentType = os.getenv("CONTENT_TYPE")

# if request_method == "POST" and contentType and  "multipart/form-data" or "image\png" or "application/octet-stream" in contentType:
# 	upload_file()
# else
# 	print("Content-Type: text/html\n")
#     print("<html><body><h1>Unsupported request method</h1></body></html>")

# if __name__ == "__main__":
#     main()

import os
import cgi
import uuid

UPLOAD_DIR = os.getenv("UPLOAD_DIR")

def get_file_extension(content_type):
    # Map common content types to file extensions
    mime_map = {
        "image/png": ".png",
        "image/jpeg": ".jpg",
        "image/gif": ".gif",
        "application/pdf": ".pdf",
        "text/plain": ".txt",
    }
    return mime_map.get(content_type, "")  # Default to no extension if unknown

def upload_file():
    if not os.path.exists(UPLOAD_DIR):
        os.makedirs(UPLOAD_DIR)  # Ensure the upload directory exists

    # Get the form data
    form = cgi.FieldStorage()

    # Iterate over all fields in the form
    for field_name in form.keys():
        file_item = form[field_name]

        # Check if the field contains a file
        if file_item.file:
            # Determine the filename
            if file_item.filename:  # If a filename is provided
                filename = os.path.basename(file_item.filename)
            else:  # If no filename is provided
                content_type = file_item.type  # Get the content type
                extension = get_file_extension(content_type)  # Get extension
                filename = f"file_{uuid.uuid4().hex}{extension}"

            filepath = os.path.join(UPLOAD_DIR, filename)

            # Save the binary file content
            with open(filepath, 'wb') as f:
                f.write(file_item.file.read())

            # Send a success message
            print("Content-Type: text/html\n")
            print("<html><body>")
            print(f"<h2>File uploaded successfully!</h2>")
            print(f"<p>Field name: {field_name}</p>")
            print(f"<p>File saved as: {filename}</p>")
            print("</body></html>")
            return

    # If no file was uploaded
    print("Content-Type: text/html\n")
    print("<html><body><h1>No file uploaded</h1></body></html>")

if __name__ == "__main__":
    upload_file()