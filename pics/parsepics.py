#!/usr/bin/python

# this script can parse serial text output dump and extract xbm images from there

from itertools import islice


match_1 = '#define xbm_width'
match_2 = '};\n'
outindex = 0
chunk_start = 0
chunk_end = 0
chunk_headline = ''

with open('dump_powrmenu.txt') as infile:
    for num, line in enumerate(infile, 1):
        if match_1 in line:
            chunk_start = num
            chunk_headline = line
            continue

        if match_2 in line:
            chunk_end = num
            print("Found chunk in lines: " + str(chunk_start) + "-" + str(chunk_end))
            with open('dump_powrmenu.txt') as infile2:
                chunk = chunk_headline + ''.join(islice(infile2, chunk_start, chunk_end))
                with open(f'out/pwr_{outindex:02d}.xbm', "w") as f_out:
                    f_out.write(chunk)
                    outindex += 1

