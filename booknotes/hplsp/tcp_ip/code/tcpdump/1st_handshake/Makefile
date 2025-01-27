all: clean

clean:
	@rm -rf *.pcap

SERVER_IP=10.207.83.17
PORT=1234
INTERFACE=ens33
LOG=1st_syn_fail.pcap

# 1. use iptables to drop the TCP packet whose destination port is $(PORT)
# 2. run server at remote machine
serve:
	sudo iptables -A INPUT -p tcp --destination-port $(PORT) -j DROP
	nc -lk -p $(PORT)

# client will exit by itself
client:
	nc -q 1 $(SERVER_IP) $(PORT)

# tcpdump at client side
monitor: clean
	sudo tcpdump -nn -i $(INTERFACE) tcp and host $(SERVER_IP) and port $(PORT) -w $(LOG)

show:
	wireshark $(LOG)&
