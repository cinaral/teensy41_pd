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

#include "MotorDriver.hpp"

#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif

MotorDriver::MotorDriver(const uint8_t PWM_pin, const uint8_t CW_pin, const uint8_t CCW_pin, Encoder *const encoder,
                         const int encoder_PPR, const int PWM_resolution, const float PWM_min, const float PWM_max)
    : PWM_pin_(PWM_pin), CW_pin_(CW_pin), CCW_pin_(CCW_pin), encoder_(encoder), encoder_PPR_(encoder_PPR),
      PWM_resolution_(PWM_resolution), PWM_min_(PWM_min), PWM_max_(PWM_max)
{
}

//* write to motor control digital pins
void
MotorDriver::command(const float u)
{
	write_direction(u);
	write_PWM(u);
}

//* stop motor
void
MotorDriver::stop()
{
	command(0.);
}

//* zero encoder
void
MotorDriver::zero_encoder()
{
	encoder_->write(0);
}

//* write to direction digital pins
void
MotorDriver::write_direction(const float u)
{
	if (u < 0. && u >= -1.) {
		digitalWrite(CW_pin_, LOW);
		digitalWrite(CCW_pin_, HIGH);
	} else if (u > 0. && u <= 1.) {
		digitalWrite(CW_pin_, HIGH);
		digitalWrite(CCW_pin_, LOW);
	} else {
		digitalWrite(CW_pin_, LOW);
		digitalWrite(CCW_pin_, LOW);
	}
}

//* write to PWM digital pins
void
MotorDriver::write_PWM(const float u)
{
	float PWM = PWM_min_ + (PWM_max_ - PWM_min_) * std::abs(u);

	if (PWM > PWM_max_) {
		PWM = PWM_max_;
	} else if (PWM < PWM_min_) {
		PWM = PWM_min_;
	}
	const int PWM_analog = static_cast<int>(PWM * PWM_resolution_);
	analogWrite(PWM_pin_, PWM_analog);
}

//* get motor position
float
MotorDriver::get_position()
{
	const float pos_radians = static_cast<float>(encoder_->read()) / encoder_PPR_ * 2. * M_PI;
	return pos_radians;
}
