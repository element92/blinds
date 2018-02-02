#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Basic code for controlling a stepper motor attached to an Easy Driver motor controller.
	Based on v4.5 of the Easy Driver.

Example:

Attributes:

.. _Google Python Style Guide:
   http://google.github.io/styleguide/pyguide.html

"""

import RPi.GPIO as GPIO
import time

class EasyDriver:
    """ Represents one EasyDriver motor controller board.
    The following pins must be connected to GPIO pins on the microcontroller:
        INSERT DESCRIPTION HERE
    """

    # Direction constants for setting the rotational direction.
    # Direction is from the motor body's point of view.
    CLOCKWISE = GPIO.HIGH
    COUNTERCLOCKWISE = GPIO.LOW

    def __init__(self, pin_enable, pin_step, pin_dir, pin_MS1, pin_MS2):
        """ Initializes a new EasyDriver object

        Args:
            pin_enable (int): GPIO pin connected to the ENABLE pin on the Easy Driver
            pin_step (int): GPIO pin connected to the STEP pin on the Easy Driver
            pin_dir (int): GPIO pin connected to the DIR pin on the Easy Driver
            pin_MS1 (int): GPIO pin connected to the MS1 pin on the Easy Driver
            pin_MS2 (int): GPIO pin connected to the MS2 pin on the Easy Driver
        """
        # Unpack the inputs
        self.pin_enable = pin_enable
        self.pin_step = pin_step
        self.pin_dir = pin_dir
        self.pin_MS1 = pin_MS1
        self.pin_MS2 = pin_MS2

        # Basic setup of GPIO access
        GPIO.setmode(GPIO.BCM)
        GPIO.setwarnings(False)
        pin_list = [pin_enable, pin_step, pin_dir, pin_MS1, pin_MS2]
        GPIO.setup(pin_list, GPIO.OUT)

        # Set to full stepping mode
        GPIO.output(pin_MS1, GPIO.LOW)
        GPIO.output(pin_MS2, GPIO.LOW)

    def enable(self):
        """ Enables power to the motor.

        Args:
            None

        Returns:
            None

        Raises:

        .. _PEP 484:
           https://www.python.org/dev/peps/pep-0484/

        """
        GPIO.output(self.pin_enable, GPIO.LOW)

    def disable(self):
        """ Disables power to the motor."""
        GPIO.output(self.pin_enable, GPIO.HIGH)

    def move(self, dir, delay, steps):
        """ Spins the motor forward """
        # Set the motor direction
        GPIO.output(self.pin_dir, dir)
        # Convert the input delay (ms) into seconds, and
        # divide by two. This is because the Easy Driver
        # only steps on a rising voltage, so we need to
        # return the pin to LOW before making another step.
        delay_mod = delay / 1000.0 / 2.0
        # Perform the indicated number of steps
        for i in range(0,steps):
            GPIO.output(self.pin_step, GPIO.HIGH)
            time.sleep(delay_mod)
            GPIO.output(self.pin_step, GPIO.LOW)
            time.sleep(delay_mod)

def main():
    """ Example usage of the Example class """
    try:
        # Code to execute
        print('Creating an EasyDriver object with default GPIO connections')

        x = EasyDriver(6,18,16,23,26)
        x.enable()

        dir = input('Enter direction.  1 for clockwise, 0 for CCW >>> ')
        delay = input('Enter the delay between steps (ms) >>> ')
        steps = input('Enter the number of steps >>> ')

        x.move(dir, delay, steps)


    except KeyboardInterrupt:
        # Code that executes after pressing CTRL+C
        print("Exiting program -- keyboard interrupt")

    except ValueError as err:
        print('Value error')
        print(err.args)

    except:
        # Code that executes after all other exceptions
        print("Exiting program -- unknown error")

    finally:
        # Code that always executes at the end of a program
        print('Exiting program')
        x.disable()

if __name__ == "__main__":
    main()
