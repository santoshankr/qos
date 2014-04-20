

#! /bin/bash
if [ $# -lt 2 ] 
	then
		echo "Usage: $0 bw_fc_0 bw_fc_1	   0 < bw_fc_i <= 1"
			exit
			fi
			
			#
			# This is kilobits per second
			#
			CAPACITY=4096
			SCALE=256
			BW_FC_1=$1
			BW_FC_2=$2
			
			BW_1=$[BW_FC_1 * CAPACITY / SCALE ]
			BW_2=$[BW_FC_2 * CAPACITY / SCALE ]
			
			IPT=$(which iptables)
			IPTADD="$IPT -t mangle -A POSTROUTING -o $IFACE"
			IPTDEL="$IPT -t mangle -D POSTROUTING -o $IFACE"
			
			tc qdisc del dev br-lan root
			tc qdisc add dev br-lan root handle 1: htb default 30
			
			tc class add dev br-lan parent 1:  classid 1:1 htb rate $CAPACITY
			
		 	tc class add dev br-lan parent 1:1 classid 1:2 htb rate ${BW_1}kbit burst 30k
			tc qdisc add dev br-lan parent 1:2 tbf rate 2mbit burst 10kb
			
			tc class add dev br-lan parent 1:1 classid 1:3 tbf rate ${BW_2}kbit burst 30k
			tc qdisc add dev br-lan parent 1:3 tbf rate 2mbit burst 10kb
			
			$IPTDEL -d 192.168.142.130 -j CLASSIFY --set-class 1:2
			$IPTADD -d 192.168.142.130 -j CLASSIFY --set-class 1:2
			
			$IPTDEL -d 192.168.142.172 -j CLASSIFY --set-class 1:3"
			$IPTADD -d 192.168.142.172 -j CLASSIFY --set-class 1:3"
