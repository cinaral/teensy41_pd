/*
 * teensy_pd
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

#include "config.hpp"
#include "cxxopts.hpp"
#include "print_echo_resp.hpp"
#include "udp_msg.hpp"
#include <chrono>
#include <cstdio>

using sock_T = udp_msg::sock<config::Flag, float, 1, config::val_dim>;

const std::string default_IP = "192.168.1.10";
const std::string default_flag{static_cast<unsigned char>(config::Flag::stop)};
const std::string default_value = std::to_string(.0f);
const std::string default_port = std::to_string(config::teensy_port);
const std::string default_timeout = std::to_string(10000);

void wait_echo_resp(sock_T &from_soc, int timeout_duration);

int
main(int argc, char const *argv[])
{
	cxxopts::Options options("send_udp",
	                         "Send a single 5-byte flag-value pair UDP packet to a given IP and port. \nThe "
	                         "available flags are defined in shared/config.hpp.");
	options.positional_help("[ip]");
	options.show_positional_help();
	// clang-format off
	options.add_options()
		("h,help", "Print usage")
		("ip", "Receiver IP Address (e.g. \"192.168.1.1\")", cxxopts::value<std::string>()->default_value(default_IP))
		("p,port", "Receiver Port", cxxopts::value<std::string>()->default_value(default_port))
		("f,flag", "Packet Flag", cxxopts::value<std::string>()->default_value(default_flag))
		("v,value", "Packet Value", cxxopts::value<std::string>()->default_value(default_value))
		("t,timeout", "Receive timeout in ms", cxxopts::value<std::string>()->default_value(default_timeout));
	// clang-format on
	options.parse_positional({"ip"});

	try {
		auto result = options.parse(argc, argv);

		if (result.count("help")) {
			std::cout << options.help() << std::endl;
			return 0;
		}
		const std::string hostname = result["ip"].as<std::string>();
		const unsigned port = std::stoi(result["port"].as<std::string>());
		const unsigned char flag_char = result["flag"].as<std::string>().c_str()[0];
		const float value_in = std::stof(result["value"].as<std::string>());
		const int timeout_duration = std::stoi(result["timeout"].as<std::string>());

		//* send UDP packet
		sock_T soc(hostname.c_str(), port);

		const config::Flag f = static_cast<config::Flag>(flag_char);
		static config::Flag flag[] = {f};
		static float value[config::val_dim] = {value_in};

		if (soc.send(flag, value) > 0) {
			//* print sent
			printf("Sent 0x%02x: %g to %s:%u\n", static_cast<unsigned int>(flag[0]), value[0],
			       hostname.c_str(), port);
		}
		if (flag[0] == config::Flag::echo) {
			wait_echo_resp(soc, timeout_duration);
		}
	} catch (cxxopts::option_has_no_value_exception const &) {
		std::cout << "Incorrect arguments!" << std::endl;
		std::cout << options.help() << std::endl;
		return 0;
	}

	return 0;
}

void
wait_echo_resp(sock_T &from_soc, int timeout_duration)
{
	bool was_received = false;
	config::Flag flag_recv[config::flag_dim] = {config::Flag::null};
	float echo_resp[config::val_dim];
	

	auto start = std::chrono::high_resolution_clock::now();
	auto now = std::chrono::high_resolution_clock::now();
	auto since_start = start - now;

	//* while loop on timeout
	while (since_start < std::chrono::milliseconds(timeout_duration)) {
		//* receive
		if (from_soc.receive(flag_recv, echo_resp) > 0) {
			//* print received
			init_echo_resp();
			clear_echo_resp();
			print_echo_resp(flag_recv, echo_resp);
			was_received = true;
			break;
		}
		now = std::chrono::high_resolution_clock::now();
		since_start = now - start;
	}

	if (!was_received) {
		printf("No message received after %d ms\n", timeout_duration);
	}
}
