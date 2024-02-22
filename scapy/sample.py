from scapy.all import *
import sys


def packet_handler(packet):
    if packet.haslayer(UDP) and packet[UDP].dport == 8888:
        print(packet[Raw].load.decode("utf-8"))


try:
    sniff(filter="udp port 8888", prn=packet_handler)
except KeyboardInterrupt:
    print("Script interrupted by keyboard")
    sys.exit(0)