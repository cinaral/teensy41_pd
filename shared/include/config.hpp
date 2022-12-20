/*
 * teensy_pd/shared
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

#ifndef CONFIG_HPP_CINARAL_221002_1402
#define CONFIG_HPP_CINARAL_221002_1402

#include <cmath>
#include <cstdint>
#include <string>

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

namespace config
{
constexpr unsigned one_sec_in_ms = 1e3; //* [ms]
constexpr unsigned one_sec_in_us = 1e6; //* [us]
enum class State : unsigned char {
	standby = 0x20,
	track = 0x21,
	eth_io = 0x22,
};

//* Motor driver configuration:
constexpr unsigned enc_A_pin = 2;          //* IN - encoder A
constexpr unsigned enc_B_pin = 3;          //* IN - encoder B
constexpr unsigned PWM_pin = 4;            //* OUT - PWM
constexpr unsigned CW_pin = 5;             //* OUT - CW
constexpr unsigned CCW_pin = 6;            //* OUT - CCW
constexpr unsigned encoder_PPR = 4000;     //* encoder points per rotation e.g. 4 * 1280 for T's encoder
constexpr float PWM_min = .0;              //* [0, 1]
constexpr float PWM_max = 1.;              //* [0, 1]
constexpr unsigned analog_resolution = 10; //* number of bits
constexpr unsigned PWM_resolution = static_cast<int>(std::pow(2, analog_resolution) - 1); //* resolution of PWM

//* Controller configuration
constexpr unsigned standby_LED_delay_ms = 500; //* [ms]
constexpr unsigned sample_freq = 1e3;          //* [hz]
constexpr float sample_step = 1. / static_cast<float>(sample_freq);
constexpr int sample_step_us = static_cast<int>(one_sec_in_us * sample_step); //* [us]
constexpr float alpha_pos_pred_motor = 200;
constexpr float pos_filter_const = 250;
constexpr float vel_filter_const = 250;
constexpr float pos_min = -M_PI;
constexpr float pos_max = M_PI;
constexpr float cur_min = -6;
constexpr float cur_max = 6;
constexpr float K_p_0 = 1;
constexpr float K_d_0 = 0.015;

//* Ethernet communication configuration:
enum class Flag : unsigned char {
	null = 0x0,
	stop = 0x30,    //* ASCII: 0
	reset = 0x31,   //* ASCII: 1
	track = 0x32,   //* ASCII: 2
	set_r = 0x33,   //* ASCII: 3
	set_K_p = 0x34, //* ASCII: 4
	set_K_d = 0x35, //* ASCII: 5
	echo = 0x36,    //* ASCII: 6
	interrupt_begin = 0x60,
	echo_resp = 0x61
};

const char computer_IP[] = "10.66.71.254";
const char teensy_IP[] = "192.168.1.10";
constexpr unsigned computer_port = 6667;
constexpr unsigned teensy_port = 11337;
constexpr unsigned dhcp_timeout = 20000; //* 20 seconds
constexpr unsigned queue_size = 10;      //* number of packets to queue
constexpr unsigned flag_dim = 1;
constexpr unsigned val_dim = 16;
constexpr size_t echo_size = 8;
constexpr size_t flag_size = sizeof(Flag[flag_dim]); //* flag size in bytes
constexpr size_t val_size = sizeof(float[val_dim]);  //* var size in bytes
constexpr unsigned msg_size = flag_size + val_size;  //* message size in bytes

static_assert(echo_size <= val_size, "echo_size must be smaller than and equal to val_size");

//* Assertions
static_assert(PWM_min >= 0. && PWM_min <= 1., "PWM_min must be in [0, 1]");
static_assert(PWM_max >= 0. && PWM_max <= 1., "PWM_max must be in [0, 1]");
static_assert(PWM_max > PWM_min, "PWM_max must be greater than PWM_min");
} // namespace config

#endif
