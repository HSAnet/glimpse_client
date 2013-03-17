from twisted.web.resource import Resource
import simplejson
import sqlite3
import time

# The registration page
class Registration(Resource):
    isLeaf = True

    def __init__(self):
        Resource.__init__(self)
        self.cache = TrackerCache()

    def render_GET(self, request):
        return self.render_POST(request)

    def render_POST(self, request):
        #request.setHeader("Content-Type", "text/json")

        try:
            for ip in request.args["ip"]:
                # Read the incoming data
                print "Got registration from %s" % ip
                self.cache.addIp(ip)

            # We also add the public ip
            self.cache.addIp(request.getClientIP())
        except KeyError:
            pass

        # Return other values
        others = self.cache.getIpList("")
        return simplejson.dumps(others)

class Unregister(Resource):
    def __init__(self):
        Resource.__init__(self)

# TODO: Save to database
# TODO: Time to live entries
class TrackerCache(object):
    def __init__(self):
        self.ipList = set()
        self.db = sqlite3.connect("tracker.db")

        self.setupDb()

    def setupDb(self):
        c = self.db.cursor()
        c.execute('''CREATE TABLE IF NOT EXISTS entries
                     (ip text PRIMARY KEY, timestamp integer, ttl integer)''')

        self.db.commit()

    def addIp(self, ip, ttl=300):
        self.ipList.add(ip)

        timestamp = time.time()

        c = self.db.cursor()
        c.execute('''INSERT INTO entries (ip, timestamp, ttl)
                     VALUES (?, ?, ?)''',
                  (ip, timestamp, ttl))

        self.db.commit()

    def getIpList(self, myip):
        lst = set(self.ipList)

        try:
            lst.remove(myip)
        except KeyError:
            pass

        return list(lst)

