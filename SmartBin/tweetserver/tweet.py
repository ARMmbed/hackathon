#!/usr/bin/env python3
import tweepy, sys, random
import http.server
import socketserver
from time import sleep

# Twitter Setup
argfile = str(sys.argv[1])

CONSUMER_KEY = 'xxxx'
CONSUMER_SECRET = 'xxxx'
ACCESS_KEY = 'xxxx'
ACCESS_SECRET = 'xxxx'
auth = tweepy.OAuthHandler(CONSUMER_KEY, CONSUMER_SECRET)
auth.set_access_token(ACCESS_KEY, ACCESS_SECRET)
api = tweepy.API(auth)

def randomFullTweet():
	filename = open("tweets.txt",'r')
	f = filename.readlines()
	filename.close()

	api.update_status(status=random.choice(f))

# HTTP Setup

DUMMY_RESPONSE = "Tweet sent."

class MyHandler(http.server.SimpleHTTPRequestHandler):
	def __init__(self, req, client_addr, server):
		http.server.SimpleHTTPRequestHandler.__init__(self, req, client_addr, server)

	def do_GET(self):
		self.send_response(200)
		self.send_header("Content-type", "text/html")
		self.send_header("Content-length", len(bytes(DUMMY_RESPONSE, 'UTF-8')))
		self.end_headers()
		self.wfile.write(bytes(DUMMY_RESPONSE, 'UTF-8'))
		print(DUMMY_RESPONSE)
		randomFullTweet()


PORT = 8080
Handler = MyHandler
httpd = socketserver.TCPServer(("", PORT), Handler)
print("serving at port", PORT)


httpd.serve_forever()
