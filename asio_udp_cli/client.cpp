#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <boost/asio.hpp>

using boost::asio::ip::udp;
using std::endl;

enum { max_length = 655035 };

int main(int argc, char *argv[])
{
	//const char *host = "192.168.1.103";
	//const char *host = "127.0.0.1";
	//const char *port = "9000";
	if (argc < 3) {
		std::cout << "Usage: ./client <host> <port>" << std::endl;
		std::cout << "or ./client <host> <port> <msg> <msgbytes> <count> " << std::endl;
		return 1;
	}
	const char *host = argv[1];
	const char *port = argv[2];
	boost::asio::io_service io_service;

	std::string message;

	bool interactive=true;

	size_t bytes, count, request_length;
	if (argc >= 2+3+1) {
		interactive=false;
		message = argv[3];
		bytes = atoi(argv[4]);
		count = atoi(argv[5]);
	}

	udp::socket s(io_service, udp::endpoint(udp::v4(), 0));

	udp::resolver resolver(io_service);
	udp::endpoint endpoint = *resolver.resolve({udp::v4(), host, port});

	std::cout << endl << "Usage example: give command: " << endl 
		<< "  foo 0 1    - this will send text foo (1 time)" << std::endl
		<< "  abc 50 1000   - this will send message of letter 'a' repeated 50 times. This msg will be sent 1000 times."
		<<std::endl;

	std::cout << std::endl;

	while(true) {
		char request[max_length];
		if (interactive) {
			std::cout << "Enter: message bytes count";
			std::cout << std::endl << "-> ";
			std::cin >> message >> bytes >> count;
		}
		if (message.size() > max_length) {
			std::cout << "too long message" << std::endl;
			continue;
		}
		if (bytes == 0) {
			request_length = message.size();
		}
		else {
			request_length = bytes;
			assert(message.size()>=1);
			char ch = message.at(0);
			std::fill_n(request, request_length, ch);
		}

		std::cerr << "Sending now " << count << " time(s) an UDP packet ";
		std::cerr << "datagram: size " << request_length << " B ";
		std::cerr << "data begins with \"" 
			<< std::string(request, request + std::min(static_cast<size_t>(10),request_length))
			<< "\"" << std::endl;
		std::cerr << endl;
		for (size_t i=0; i<count; i++) {
			s.send_to(boost::asio::buffer(request, request_length), endpoint);
		}
		if (!interactive) break;
	}
/*
	char reply[max_length];
	udp::endpoint sender_endpoint;
	size_t reply_length = s.receive_from(
			boost::asio::buffer(reply, max_length), sender_endpoint);
	std::cout << "Reply is: ";
	std::cout.write(reply, reply_length);
	std::cout << "\n";
*/
return 0;
}
