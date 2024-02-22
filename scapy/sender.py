from scapy.all import *

def send_video_packets(video_data, destination_ip, destination_port):
    packet_size = 1024  # Adjust this value based on your requirements
    packets = [video_data[i:i+packet_size] for i in range(0, len(video_data), packet_size)]

    for i, packet_data in enumerate(packets):
        packet = IP(dst=destination_ip)/UDP(dport=destination_port)/Raw(load=packet_data)
        send(packet)
        print(f"Sent packet {i+1}/{len(packets)}")

# Example usage
video_data = b"Your generated video content goes here"
destination_ip = "127.0.0.1"  # Replace with the destination IP address
destination_port = 8888

send_video_packets(video_data, destination_ip, destination_port)