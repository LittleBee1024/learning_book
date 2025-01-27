all: clean

clean:
	@rm -rf *.pcap

SERVER_IP=10.207.83.17
PORT=1234
INTERFACE=ens33

# run server at another machine
serve:
	nc -lk -p $(PORT)

# "Ctrl+D" command quits after 1 second
client:
	nc -q 1 $(SERVER_IP) $(PORT)

# tcpdump at client side
monitor: clean
	sudo tcpdump -nn -i $(INTERFACE) tcp and host $(SERVER_IP) and port $(PORT) -w normal.pcap

show:
	wireshark normal.pcap&
