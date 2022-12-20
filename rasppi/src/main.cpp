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
#include "print_echo_resp.hpp"
#include "udp_msg.hpp"
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <signal.h>
#include <string>

using sock_T = udp_msg::sock<config::Flag, float, config::flag_dim, config::val_dim>;

constexpr float sine_freq = .5;
constexpr uint32_t sample_freq = 1e2; //* [hz]
constexpr uint32_t print_freq = 60;   //* [hz]
constexpr float sample_step = 1. / static_cast<float>(sample_freq);
constexpr float print_step = 1. / static_cast<float>(print_freq);
constexpr uint32_t task_duration = 180 * config::one_sec_in_us;                                 //* [us]
constexpr uint32_t sample_step_us = static_cast<uint32_t>(config::one_sec_in_us * sample_step); //* [us]
constexpr uint32_t print_step_us = static_cast<uint32_t>(config::one_sec_in_us * print_step);   //* [us]

void relay_echo_resp(sock_T &from_soc, sock_T &dest_soc);

int
main(int, char const *[])
{
	init_echo_resp();

	sock_T teensy_soc(config::teensy_IP, config::teensy_port);
	sock_T PC_soc(config::computer_IP, config::computer_port);

	config::Flag flag_sent[config::flag_size] = {config::Flag::reset};
	float val_sent[config::val_dim];

	if (teensy_soc.send(flag_sent, val_sent) < 0) {
		printf("Failed to reset.");
	}

	flag_sent[0] = config::Flag::track;

	if (teensy_soc.send(flag_sent, val_sent) < 0) {
		printf("Failed to start tracking.");
	}
	bool did_interrupt_begin = false;

	config::Flag flag_recv[config::flag_dim] = {config::Flag::null};
	float val_recv[config::val_dim];

	auto update_time = std::chrono::high_resolution_clock::now();
	auto print_time = std::chrono::high_resolution_clock::now();
	auto start_time = std::chrono::high_resolution_clock::now();
	auto now = std::chrono::high_resolution_clock::now();
	auto since_update = now - update_time;
	auto since_print = now - print_time;
	auto since_start = now - start_time;
	float elapsed = 0;
	float r = 0;
	bool is_running = true;

	//* wait for interrupt to begin for accurate timing
	while (!did_interrupt_begin) {
		teensy_soc.receive(flag_recv, val_recv);

		if (flag_recv[0] == config::Flag::interrupt_begin) {
			did_interrupt_begin = true;
		}
	}
	now = std::chrono::high_resolution_clock::now();

	while (is_running) {
		now = std::chrono::high_resolution_clock::now();
		since_update = now - update_time;
		since_print = now - print_time;
		since_start = now - start_time;
		elapsed = std::chrono::duration<double>(since_start).count();

		if (since_update >= std::chrono::microseconds(sample_step_us)) {
			update_time = std::chrono::high_resolution_clock::now();
			r = std::sin(elapsed * 2 * M_PI * sine_freq);
			flag_sent[0] = config::Flag::track;
			val_sent[0] = r;
			teensy_soc.send(flag_sent, val_sent);
		}

		if (since_print >= std::chrono::microseconds(print_step_us)) {
			print_time = std::chrono::high_resolution_clock::now();	
			relay_echo_resp(teensy_soc, PC_soc);
		}

		if (since_start >= std::chrono::microseconds(task_duration)) {
			flag_sent[0] = config::Flag::stop;
			teensy_soc.send(flag_sent, val_sent);
			is_running = false;
			relay_echo_resp(teensy_soc, PC_soc);
		}
	}
	return 0;
}

void
relay_echo_resp(sock_T &from_soc, sock_T &dest_soc)
{
	static config::Flag flag_recv[config::flag_dim] = {config::Flag::null};
	static float echo_resp[config::val_dim];

	//* request echo
	static config::Flag flag_sent[config::flag_size] = {config::Flag::echo};
	static float val_sent[config::val_dim];
	from_soc.send(flag_sent, val_sent);

	if (from_soc.receive(flag_recv, echo_resp) > 0) {
		//* print received
		clear_echo_resp();
		print_echo_resp(flag_recv, echo_resp);
		//* relay
		dest_soc.send(flag_recv, echo_resp);
	}
}
