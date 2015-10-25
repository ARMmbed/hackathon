#!/usr/bin/env python

"""This script will merge two hex files and write the output to a hex file.
   USAGE: merge_hex.py input_file1 input_file2 output_file.
"""

import sys

fail_color = ''

# If colorama is present, set the fail color to red
try:
    from colorama import init, deinit, Fore
    fail_color = Fore.RED
except:
    pass

def fail(message):
    print(fail_color + message)

    # If we've included ANSI color in output, reset the output style
    if fail_color:
        print(Fore.RESET)
        deinit()

    return 1

def main(arguments):
    # If using ANSI coloring is available, initialize colorama
    if fail_color:
        init()

    # Import intelhex if avaialable, otherwise fail
    try:
        from intelhex import IntelHex
    except:
        return fail('error: You do not have \'intelhex\' installed. Please run \'pip install intelhex\' then retry.')

    # Ensure the right number of arguments are supplied
    if not len(arguments) == 3:
        return fail('error: Improper use of merge_hex.py.\nUSAGE: merge_hex.py input_file1 input_file2 output_file.')

    # Get the two hex files, merge them, and save the result
    orig = IntelHex(arguments[0])
    new = IntelHex(arguments[1])
    orig.merge(new, overlap='replace')
    orig.write_hex_file(arguments[2])

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
