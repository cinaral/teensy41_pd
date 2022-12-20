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

#ifndef UDPHANDLER_HPP_CINARAL_221102_1322
#define UDPHANDLER_HPP_CINARAL_221102_1322

#include "config.hpp"
#include <QNEthernet.h>

class UdpHandler
{
  public:
	UdpHandler(const uint16_t queue_size, const uint16_t listening_port, const uint32_t dhcp_timeout = 10000);
	void initialize();
	void read(config::Flag (&flag_arr)[config::flag_dim], float (&val_arr)[config::val_dim]);
	void write(const config::Flag (&flag_arr)[config::flag_dim], const float (&val_arr)[config::val_dim]);

  private:
	qindesign::network::EthernetUDP server_;
	const uint16_t listening_port_;
	const uint32_t dhcp_timeout_;
};

#endif
