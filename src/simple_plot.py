from pylab import *
import sys

r = open(sys.argv[1])
s = open(sys.argv[2])

rdata = [int(i.strip()) for i in r]
sdata = [int(i.strip()) for i in s]

l = min(len(rdata), len(sdata))

print rdata[:l]
print sdata[:l]
exit(0)

t = range(0, 2*l, 2)
plot(t, rdata[:l], 'r-', t, sdata[:l], 'b-')

xlabel('Time (s)')
ylabel('Throughput (bytes)')
title('Throughput plot')
grid(True)
savefig("test.png")
show()
