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

# import os
# import cgi
# import uuid
# import magic

# UPLOAD_DIR = os.getenv("UPLOAD_DIR")

# def get_file_extension(content_type):
#     # Map common content types to file extensions
#     mime_map = {
#     "image/png": ".png",
#     "image/jpeg": ".jpg",
#     "image/gif": ".gif",
#     "application/pdf": ".pdf",
#     "text/plain": ".txt",
#     "application/json": ".json",
# }
#     return mime_map.get(content_type, ".")  # Default to no extension if unknown


# def upload_file():
#     if not os.path.exists(UPLOAD_DIR):
#         os.makedirs(UPLOAD_DIR)  # Ensure the upload directory exists

#     # Get the form data
#     form = cgi.FieldStorage()

#     # Iterate over all fields in the form
#     for field_name in form.keys():
#         file_item = form[field_name]

#         # Check if the field contains a file
#         if file_item.file:
#             try:
#                 # Determine the filename
#                 if file_item.filename:  # If a filename is provided
#                     filename = os.path.basename(file_item.filename)
#                 else:  # If no filename is provided
#                     content_type = file_item.type  # Get the content type
#                     extension = get_file_extension(content_type)  # Get extension
#                     filename = f"file_{uuid.uuid4().hex}{extension}"

#                 filepath = os.path.join(UPLOAD_DIR, filename)

#                 # Save the binary file content
#                 # with open(filepath, 'wb') as f:
#                 #     f.write(file_item.file.read())
#                 # with open(filepath, 'wb' if file_item.type not in ["text/plain", "application/json"] else 'w') as f:
#                 #  content = file_item.file.read()
#                 # if isinstance(content, bytes) and file_item.type in ["text/plain", "application/json"]:
#                 #  content = content.decode('utf-8')  # Decode bytes to string for text files
#                 # f.write(content)
#               with open(filepath, 'wb' if file_item.type not in ["text/plain", "application/json"] else 'w') as f:
#                content = file_item.file.read()
    
#     # If the file type is text and the content is bytes, decode it
#               if file_item.type in ["text/plain", "application/json"]:
#                if isinstance(content, bytes):  # Decode only if content is bytes
#                 content = content.decode('utf-8')  
#                f.write(content)  # Write text content
#                else:
#                 if isinstance(content, str):  # Encode to bytes if content is a string
#                  content = content.encode('utf-8')
#                 f.write(content)  # Write binary content				

#                 # Send a success message
#                 print("<html><body>")
#                 print(f"<h2>File uploaded successfully!</h2>")
#                 print(f"<p>Field name: {field_name}</p>")
#                 print(f"<p>File saved as: {filename}</p>")
#                 print("</body></html>")
#                 print("<a href='/checkbox.html'><button>Back</button></a>")
#                 return  # Exit after successful upload
#             except Exception as e:
#                 # Handle any unexpected errors
#                 print("Content-Type: text/html\n")
#                 print("<html><body>")
#                 print(f"<h1>Error uploading file</h1>")
#                 print(f"<p>{str(e)}</p>")
#                 print("</body></html>")
#                 print("<a href='/checkbox.html'><button>Back</button></a>")
#                 return

#     # If no file was uploaded
#     print("Content-Type: text/html\n")
#     print("<html><body><h1>No file uploaded. Please select a file to upload.</h1></body></html>")
#     print("<a href='/checkbox.html'><button>Back</button></a>")
# if __name__ == "__main__":
#     upload_file()

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
        "application/json": ".json",
    }
    return mime_map.get(content_type, ".")  # Default to no extension if unknown


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
            try:
                # Determine the filename
                if file_item.filename:  # If a filename is provided
                    filename = os.path.basename(file_item.filename)
                else:  # If no filename is provided
                    content_type = file_item.type  # Get the content type
                    extension = get_file_extension(content_type)  # Get extension
                    filename = f"file_{uuid.uuid4().hex}{extension}"

                filepath = os.path.join(UPLOAD_DIR, filename)

                # Save the file content based on its type
                with open(filepath, 'wb' if file_item.type not in ["text/plain", "application/json"] else 'w') as f:
                    content = file_item.file.read()

                    # If the file type is text and the content is bytes, decode it
                    if file_item.type in ["text/plain", "application/json"]:
                        if isinstance(content, bytes):  # Decode only if content is bytes
                            content = content.decode('utf-8')
                        f.write(content)  # Write text content
                    else:
                        if isinstance(content, str):  # Encode to bytes if content is a string
                            content = content.encode('utf-8')
                        f.write(content)  # Write binary content

                # Send a success message
                print("<html><body>")
                print(f"<h2>File uploaded successfully!</h2>")
                print(f"<p>Field name: {field_name}</p>")
                print(f"<p>File saved as: {filename}</p>")
                print("</body></html>")
                print("<a href='/checkbox.html'><button>Back</button></a>")
                return  # Exit after successful upload
            except Exception as e:
                # Handle any unexpected errors
                print("Content-Type: text/html\n")
                print("<html><body>")
                print(f"<h1>Error uploading file</h1>")
                print(f"<p>{str(e)}</p>")
                print("</body></html>")
                print("<a href='/checkbox.html'><button>Back</button></a>")
                return

    # If no file was uploaded
    print("Content-Type: text/html\n")
    print("<html><body><h1>No file uploaded. Please select a file to upload.</h1></body></html>")
    print("<a href='/checkbox.html'><button>Back</button></a>")


if __name__ == "__main__":
    upload_file()