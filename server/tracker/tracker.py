from twisted.web import server, resource
from twisted.internet import reactor, ssl
from twisted.application.service import Application

from registration import *

# Build the request chain
root = resource.Resource()
root.putChild("register", Registration())
root.putChild("unregister", Unregister())

# SSL context factory
factory = ssl.DefaultOpenSSLContextFactory(
    "keys/server2.key",
    "keys/server.crt"
)

app = Application("mplane_tracker")

reactor.listenSSL(8080, server.Site(root), factory)
reactor.run()
