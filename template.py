#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Description line 1
	description line 2
	description line 3

Example:

Attributes:

.. _Google Python Style Guide:
   http://google.github.io/styleguide/pyguide.html

"""

import RPi.GPIO as GPIO

class Example:
    """ One-line description of class
    More detailed description of class.

    """

    def __init__(self, ex_arg):
        """ Initializes a new Stepper object

        Args:
            ex_arg:
                key0 (int): description
                kay1 (int): description
                key2 (int): description
        """
        # Code to run when class is initialized

    def example(self, inp1, inp2):
        """ Function description.

        Args:
            inp1 (int): description
            inp2 (float): description

        Returns:
            None

        Raises:

        .. _PEP 484:
           https://www.python.org/dev/peps/pep-0484/

        """
        # code


def main():
    """ Example usage of the Example class """
    try:
        # Code to execute
        print('hello world')

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

if __name__ == "__main__":
    main()
