tcp_server:
	clang tcp/server.c -o output/tcp_server
	./output/tcp_server
	rm ./output/tcp_server

tcp_client:
	clang tcp/client.c -o output/tcp_client
	./output/tcp_client || true
	rm ./output/tcp_client

udp_server:
	clang udp/server.c -o output/udp_server
	./output/udp_server || true
	rm ./output/udp_server

udp_client:
	clang udp/client.c -o output/udp_client
	./output/udp_client || true
	rm ./output/udp_client