#! /bin/bash
if [ $# -lt 2 ] 
	then
	echo "Usage: $0 cl_ip bw_fc"
	exit
fi

#
# This is kilobits per second
#
CAPACITY=8192
SCALE=256
IFACE="eth0"

CL_IP_1=$1
BW_FC_1=$2

BW_1=$[BW_FC_1 * CAPACITY / SCALE ]

#tc qdisc list
echo tc qdisc del dev $IFACE root

#tc qdisc list
echo tc qdisc add dev $IFACE root handle 1: htb default 6

#tc qdisc list
echo tc class add dev $IFACE parent 1: classid 1:1 htb rate ${CAPACITY}kbit burst 0

#exit
echo tc class add dev $IFACE parent 1:1 classid 1:2 htb rate ${BW_1}kbit burst 0
echo tc filter add dev $IFACE protocol ip parent 1:0 prio 0 u32 match ip dst $CL_IP_1 flowid 1:2
