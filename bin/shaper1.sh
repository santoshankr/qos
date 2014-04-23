#! /bin/bash

CAPACITY=6144
CL_IP_1=$1
BW_FC_1=$2
BW_1=$((BW_FC_1 * CAPACITY / SCALE))
IF="eth0"

tc qdisc del dev $IF root
tc qdisc add dev $IF root handle 1: htb default 6
tc class add dev $IF parent 1: classid 1:1 htb rate ${BW_1}kbit burst 0
tc class add dev $IF parent 1:1 classid 1:2 htb rate ${BW_1}kbit burst 0
tc qdisc add dev $IF parent 1:2 handle 2: sfq perturb 10
tc filter add dev $IF protocol ip parent 1:0 prio 0 u32 match ip dst $CL_IP_1 flowid 1:2
