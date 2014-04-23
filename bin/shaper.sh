#! /bin/ash
if [ $# -lt 2 ]
        then
                echo "Usage: $0 cl_ip_0 bw_fc_0 cl_ip_1 bw_fc_1    0 < bw_fc_i <= 1"
                exit
        fi


# This is kilobits per second

CAPACITY=6144
SCALE=256
IFACE="br-lan"

CL_IP_1=$1
BW_FC_1=$2

CL_IP_2=$3
BW_FC_2=$4

BW_1=$((BW_FC_1 * CAPACITY / SCALE ))
BW_2=$((BW_FC_2 * CAPACITY / SCALE ))

tc qdisc del dev $IF root
tc qdisc add dev $IF root handle 1: htb default 6
tc class add dev $IF parent 1: classid 1:1 htb rate ${CAPACITY}kbit burst 0
tc class add dev $IF parent 1:1 classid 1:2 htb rate ${BW_1}kbit burst 0
tc class add dev $IF parent 1:1 classid 1:3 htb rate ${BW_2}kbit burst 0
tc qdisc add dev $IF parent 1:2 handle 2: sfq perturb 10
tc qdisc add dev $IF parent 1:3 handle 3: sfq perturb 10
tc filter add dev $IF protocol ip parent 1:0 prio 0 u32 match ip dst $CL_IP_1 flowid 1:2
tc filter add dev $IF protocol ip parent 1:0 prio 0 u32 match ip dst $CL_IP_2 flowid 1:3
