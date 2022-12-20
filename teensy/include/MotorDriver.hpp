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

#ifndef MOTORDRIVER_HPP_CINARAL_221004_1207
#define MOTORDRIVER_HPP_CINARAL_221004_1207

#include <Arduino.h>
#include <Encoder.h>
#include <cmath>

class MotorDriver
{
  public:
	MotorDriver(const uint8_t PWM_pin, const uint8_t CW_pin, const uint8_t CCW_pin, Encoder *const encoder,
	            const int encoder_PPR, const int PWM_resolution, const float PWM_min = .0,
	            const float PWM_max = 1.);
	void command(const float input);
	void stop();
	void zero_encoder();
	float get_position();

  private:
	void write_direction(const float u);
	void write_PWM(const float u);

	const uint8_t PWM_pin_;
	const uint8_t CW_pin_;
	const uint8_t CCW_pin_;
	Encoder *const encoder_;
	const int encoder_PPR_;
	const int PWM_resolution_;
	const float PWM_min_;
	const float PWM_max_;
};

#endif
