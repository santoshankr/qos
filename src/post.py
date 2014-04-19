#!/usr/bin/python

import requests
import sys

url = 'http://127.0.0.1:8000/cgi-bin/register.cgi'

#
# Run as :
# ./post.py --urls=u1,u2,u3 --bitrates=b1,b2,b3 --screensize=2000
#

from optparse import OptionParser
parser = OptionParser()
parser.add_option("-u", "--urls", dest="urls", help="URL sources")
parser.add_option("-b", "--bitrates", dest="bitrates", help="Bitrates")
parser.add_option("-s", "--screensize", dest="screensize", help="Screensize")

(options, args) = parser.parse_args()

urls = options.urls.split(',')
bitrates = options.bitrates.split(',')

N = min(len(urls), len(bitrates))

data = str(N) + '\n'
data += options.screensize + '\n'
for i in xrange(N):
	data += urls[i] + ' ' + bitrates[i] + '\n'

r = requests.post(url, data)

print 'Response:'
print r.content