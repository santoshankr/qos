#! /bin/ash

CAPACITY=6144
CL_IP_1=$1


tc qdisc del dev br-lan root
tc qdisc add dev br-lan root handle 1: htb default 6
tc class add dev br-lan parent 1: classid 1:1 htb rate ${CAPACITY}kbit burst 0
tc class add dev br-lan parent 1:1 classid 1:2 htb rate ${CAPACITY}kbit burst 0
tc qdisc add dev br-lan parent 1:2 handle 2: sfq perturb 10
tc filter add dev br-lan protocol ip parent 1:0 prio 0 u32 match ip dst $CL_IP_1 flowid 1:2