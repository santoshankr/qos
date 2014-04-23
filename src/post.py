#!/usr/bin/python

import requests
import sys

url = 'http://192.168.142.1/cgi-bin/register.cgi?%s'

#
# Run as :
# ./post.py --urls=u1,u2,u3 --bitrates=b1,b2,b3 --screensize=2000 -i <fake_ip_address>
#

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-u", "--urls", dest="urls", help="URL sources")
parser.add_option("-b", "--bitrates", dest="bitrates", help="Bitrates")
parser.add_option("-s", "--screensize", dest="screensize", help="Screensize")
parser.add_option("-i", "--ipaddress", dest="ipaddress", help="Fake IP")

(options, args) = parser.parse_args()

urls = options.urls.split(',')
bitrates = options.bitrates.split(',')

N = min(len(urls), len(bitrates))

data = str(N) + '\n'
data += options.screensize + '\n'
for i in xrange(N):
	data += urls[i] + '\t' + bitrates[i] + '\n'

r = requests.post(url % options.ipaddress, data)

print 'Response:'
print r.content
