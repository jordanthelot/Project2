import subprocess
#allow the server to run a command (./server "clubName", which our c++ function will capture) and return the output
from http.server import BaseHTTPRequestHandler, HTTPServer
#Essentially a http server that listens for requests and responds with the output of a command
from urllib.parse import urlparse, parse_qs
#reads the url, since the clubname the usertypes will be sent to this program in a url, and we need to parse it to get the command to run

#Now it's time to create the server, and what will happen when it receives what the user typed
class RequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        #parse the url to get the command to run
        parsed_url = urlparse(self.path)
        query_params = parse_qs(parsed_url.query)
        command = query_params.get('command', [''])[0]

        result = subprocess.run(["./server", command], capture_output=True, text=True)
        #this is what runs the c++ BST function(to find the respective ELO), and captures the output to send back to the client

        self.send_response(200)
        #standard http response code for success
        self.send_header('Content-type', 'text/plain')
        #Standard -Tells the browser that the response is plain text, which is what our c++ function will return (final elorating will have to be converted back to string)
        self.send_header('Access-Control-Allow-Origin', '*')
        #Standard - CORS header, Cross-Origin resource sharing, this essentially alows the funcitonality to be ran from anywhere
        self.end_headers()
        #Actually very similar to n8n, above are the headers sent to the browser to allow the data to be authenticated, here is where the headers end
        self.wfile.write(result.stdout.encode())
        #Writes the output of the command (the output of the c++ function) back to the browser


HTTPServer(('localhost', 8080), RequestHandler).serve_forever()
#This is the actual server, and the command that keeps it running

#Essentially, point of the file is to run our own local server through python's built in functionality, capture what the user wrote in the textbox, parse through it appropriately, and sending it back to our C++ File (conti)
#In order to activate the BST lookup, which will then lookup the clubname and their associated ELO, which will then be given back to our index.html file