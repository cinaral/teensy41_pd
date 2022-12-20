/*
 * teensy_pd/teensy
 *
 * MIT License
 *
 * Copyright (c) 2022 Cinar, A. L.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "UdpHandler.hpp"
#include "include/udp_msg/pack_msg.hpp"
#include "include/udp_msg/unpack_msg.hpp"

namespace qn = qindesign::network;

union float_union {
	float f;
	char b[sizeof(float)];
};

UdpHandler::UdpHandler(const uint16_t queue_size, const uint16_t listening_port, const uint32_t dhcp_timeout)
    : server_(queue_size), listening_port_(listening_port), dhcp_timeout_(dhcp_timeout)
{
}

void
UdpHandler::initialize()
{
	qindesign::network::stdPrint = &Serial; //* Make printf work (a QNEthernet feature)

	if (!qn::Ethernet.begin()) {
		printf("Failed to start Ethernet\n");
		return;
	}

	if (!qn::Ethernet.waitForLocalIP(dhcp_timeout_)) {
		printf("Failed to get IP address from DHCP\n");
	} else {
		IPAddress ip = qn::Ethernet.localIP();
		printf("    Local IP    = %u.%u.%u.%u\n", ip[0], ip[1], ip[2], ip[3]);
		ip = qn::Ethernet.subnetMask();
		printf("    Subnet mask = %u.%u.%u.%u\n", ip[0], ip[1], ip[2], ip[3]);
		ip = qn::Ethernet.gatewayIP();
		printf("    Gateway     = %u.%u.%u.%u\n", ip[0], ip[1], ip[2], ip[3]);
		ip = qn::Ethernet.dnsServerIP();
		printf("    DNS         = %u.%u.%u.%u\n", ip[0], ip[1], ip[2], ip[3]);
		printf("Listening for clients on port %u...\n", listening_port_);

		server_.begin(listening_port_);
	}
}

void
UdpHandler::read(config::Flag (&flag_arr)[config::flag_dim], float (&val_arr)[config::val_dim])
{
	static char msg_recv[config::msg_size]; //* buffer to hold incoming packet,

	if (server_.parsePacket() > 0) {
		server_.read(msg_recv, config::msg_size); //* receive a packet over UDP
		udp_msg::unpack_msg<config::Flag>(msg_recv, flag_arr, val_arr);
	}
}

void
UdpHandler::write(const config::Flag (&flag_arr)[config::flag_dim], const float (&val_arr)[config::val_dim])
{
	static char msg[config::msg_size]; //* buffer to hold incoming packet,

	udp_msg::pack_msg<config::Flag>(flag_arr, val_arr, msg);
	server_.send(server_.remoteIP(), server_.remotePort(), reinterpret_cast<unsigned char *>(msg),
	             config::msg_size); //* and then send back
}