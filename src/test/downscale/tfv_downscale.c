/*
Tinkervision - Vision Library for https://github.com/Tinkerforge/red-brick
Copyright (C) 2014 philipp.kroos@fh-bielefeld.de

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <stdio.h>
#include <unistd.h> /* sleep (posix) */
#include <time.h>   /* nanosleep (posix) */

#include "tinkervision.h"

int main(int argc, char* argv[]) {
    TFV_Id id = 0;
    TFV_Result result = TFV_INTERNAL_ERROR;
    TFV_Word factor = 1;

    result = camera_available();
    if (result != 0) {
        printf("Requested camera not available: %s\n", result_string(result));
        return -1;
    }

    sleep(1);

    result = module_start("downscale", id);

    printf("Configured downscale id %d: Code %d (%s)\n", id, result,
           result_string(result));

    result = set_parameter(id, "factor", factor);

    printf("Set downscale-factor to %d: Code %d (%s)\n", factor, result,
           result_string(result));

    sleep(10);

    result = module_remove(id);
    printf("Stopped downscale %d: Code %d (%s)\n", id, result,
           result_string(result));

    quit();

    return 0;
}