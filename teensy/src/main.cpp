/*
 * teensy-pd_control-udp/teensy
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

#include "MotorDriver.hpp"
#include "UdpHandler.hpp"
#include "config.hpp"
#include "pid_control.hpp"
#include <Arduino.h>

IntervalTimer sample_interrupt;  //* interrupts
elapsedMicros sample_elapsed_us; //* sample timer

config::State cmd_state;  //* commanded program state
config::State state;      //* program state
bool did_interrupt_begin; //* flag for interrupt begin

config::Flag flag[config::flag_dim]; //* flag buffer
float val_arr[config::val_dim];      //*value buffer
long long unsigned t_idx;            //* time index
float t;                             //* time
float x;                             //* motor position
float x_f;                           //* filtered motor position
float dt_x;                          //* motor velocity
float u;                             //* control output
float r;                             //* reference to track
float K_p;                           //* proportional gain
float K_d;                           //* derivative gain
long long unsigned max_sample_us;    //* maximum sample time
float error_prev = 0;                //* previous position (this could be handled by the pid controller)
float u_prev = 0;                    //* previous control (this could be handled by the pid controller)

Encoder encoder(config::enc_A_pin, config::enc_B_pin);
MotorDriver driver(config::PWM_pin, config::CW_pin, config::CCW_pin, &encoder, config::encoder_PPR,
                   config::PWM_resolution, config::PWM_min, config::PWM_max);
UdpHandler udp(config::queue_size, config::teensy_port, config::dhcp_timeout);

void handle_state();
void handle_commands();
void handle_state_change();
void reset_fun();
void standby_fun();
void track_fun();
void control_fun(const float pos_next, const float pos_desired, float *u_ptr);

//* Setup
void
setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(config::enc_A_pin, INPUT);
	pinMode(config::enc_B_pin, INPUT);
	pinMode(config::PWM_pin, OUTPUT);
	pinMode(config::CW_pin, OUTPUT);
	pinMode(config::CCW_pin, OUTPUT);
	analogWriteResolution(config::analog_resolution);
	udp.initialize();
	reset_fun();
	did_interrupt_begin = false;
}

//* Loop
void
loop()
{
	handle_state();
	handle_commands();
	handle_state_change();
}

void
handle_state()
{
	switch (state) {
	case config::State::standby:
		standby_fun();
		break;
	default:
		break;
	}
}

void
handle_commands()
{
	udp.read(flag, val_arr);

	if (flag[0] != config::Flag::null) {
		/*
		* the following introduces some latency, but it may be necessary for interrupt safety in the future
		sample_interrupt.end();
		state = config::State::eth_io;
		*/

		switch (flag[0]) {
		case config::Flag::stop: {
			sample_interrupt.end();
			cmd_state = config::State::standby;
			break;
		}
		case config::Flag::reset: {
			reset_fun();
			break;
		}
		case config::Flag::track: {
			cmd_state = config::State::track;
			r = val_arr[0];
			break;
		}
		case config::Flag::set_r: {
			r = val_arr[0];
			break;
		}
		case config::Flag::set_K_p: {
			K_p = val_arr[0];
			break;
		}
		case config::Flag::set_K_d: {
			K_d = val_arr[0];
			break;
		}
		case config::Flag::echo: {
			const config::Flag flag_sent[config::flag_dim] = {config::Flag::echo_resp};
			const float val_sent[config::val_dim] = {static_cast<float>(t_idx),        t, x, u, r, K_p, K_d,
			                                         static_cast<float>(max_sample_us)};
			udp.write(flag_sent, val_sent);
			break;
		}
		case config::Flag::interrupt_begin: {
			break;
		}
		case config::Flag::echo_resp: {
			break;
		}
		default: {
			break;
		}
		}
	}
}

void
handle_state_change()
{
	if (cmd_state != state) {
		state = cmd_state;

		switch (state) {
		case config::State::standby:
			digitalWrite(LED_BUILTIN, LOW);
			did_interrupt_begin = false;
			break;
		case config::State::track:
			digitalWrite(LED_BUILTIN, HIGH);
			sample_interrupt.begin(track_fun, config::sample_step_us);
			sample_elapsed_us = 0;

			if (!did_interrupt_begin) {
				did_interrupt_begin = true;
				config::Flag flag_sent[config::flag_dim] = {config::Flag::interrupt_begin};
				float val_sent[config::val_dim] = {0};
				udp.write(flag_sent, val_sent);
			}
			break;
		default:
			break;
		}
	}
}

//* Re-initialize all parameters, reset the encoder, and go into standby.
void
reset_fun()
{
	sample_interrupt.end();
	driver.stop();
	driver.zero_encoder();
	t_idx = 0;
	t = 0;
	x = 0;
	x_f = 0;
	dt_x = 0;
	u = 0;
	r = 0;
	K_p = config::K_p_0;
	K_d = config::K_d_0;
	max_sample_us = 0;
	state = config::State::standby;
	cmd_state = config::State::standby;
	did_interrupt_begin = false;
}

//* Standby
void
standby_fun()
{
	driver.stop();
	digitalWrite(LED_BUILTIN, HIGH);
	delay(config::standby_LED_delay_ms);
	digitalWrite(LED_BUILTIN, LOW);
	delay(config::standby_LED_delay_ms);
}

//* Track reference position
void
track_fun()
{
	x = driver.get_position();
	control_fun(x, r, &u);
	u = u / config::cur_max; //* temp fix

	driver.command(u);
	++t_idx;
	t += static_cast<float>(sample_elapsed_us) / static_cast<float>(config::one_sec_in_us);

	if (sample_elapsed_us > max_sample_us) {
		max_sample_us = sample_elapsed_us;
	}
	sample_elapsed_us = 0;
}

//* Controller
void
control_fun(const float pos, const float desired_pos, float *u_ptr)
{
	const float error = pos - desired_pos; //* error
	pid_control::PDF<config::u_dim>(config::sample_step, config::filter_step, &K_p, &K_d, &error_prev, &error,
	                                &u_prev, u_ptr);
	error_prev = error;
	u_prev = *u_ptr;
}
