/*
 * teensy-pd_control-udp/udp_control
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

#ifndef PRINT_ECHO_RESP_HPP_CINARAL_221201_1535
#define PRINT_ECHO_RESP_HPP_CINARAL_221201_1535

#include "config.hpp"

inline void
init_echo_resp()
{
	//* printing buffer
	for (size_t i = 0; i < config::echo_size; ++i) {
		printf("\n");
	}
}

inline void
clear_echo_resp()
{
	//* clear the printed telemetry
	for (size_t i = 0; i < config::echo_size; ++i) {
		printf("\033[F\033[2K\r"); //* move the cursor up then clear the line
	}
}

inline void
print_echo_resp(config::Flag flag_recv[config::flag_dim], float val_recv[config::val_dim])
{
	if (flag_recv[0] == config::Flag::echo_resp) {

		bool no_match = false;

		for (size_t i = 0; i < config::val_dim; ++i) {

			//* print known flags
			switch (i) {
			case 0: {
				printf("%16s", "t_idx");
				break;
			}
			case 1: {
				printf("%16s", "t");
				break;
			}
			case 2: {
				printf("%16s", "x");
				break;
			}
			case 3: {
				printf("%16s", "u");
				break;
			}
			case 4: {
				printf("%16s", "r");
				break;
			}
			case 5: {
				printf("%16s", "K_p");
				break;
			}
			case 6: {
				printf("%16s", "K_d");
				break;
			}
			case 7: {
				printf("%16s", "max_sample_us");
				break;
			}
			default: {
				no_match = true;
				break;
			}
			}
			if (!no_match) {
				printf(": %g\n", val_recv[i]);
			}
		}
	}
}

#endif