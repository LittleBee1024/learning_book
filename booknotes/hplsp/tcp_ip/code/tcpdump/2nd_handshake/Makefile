all: clean

clean:
	@rm -rf *.pcap

SERVER_IP=10.207.83.17
PORT=1234
INTERFACE=ens33
LOG=2nd_syn_fail.pcap

serve:
	nc -lk -p $(PORT)

# 1. use iptables to drop the TCP packet from server
# 2. run the client to connect to the server
client:
	sudo iptables -I INPUT -s $(SERVER_IP) -j DROP
	nc -q 1 $(SERVER_IP) $(PORT)

# tcpdump at client side
monitor: clean
	sudo tcpdump -nn -i $(INTERFACE) tcp and host $(SERVER_IP) and port $(PORT) -w $(LOG)

show:
	wireshark $(LOG)&
