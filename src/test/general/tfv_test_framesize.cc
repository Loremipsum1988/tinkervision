/*
Tinkervision - Vision Library for https://github.com/Tinkerforge/red-brick
Copyright (C) 2014 roland.dudko@fh-bielefeld.de

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
#include <unistd.h>   /* sleep (posix) */
#include <time.h>     /* nanosleep (posix) */
#include <sys/time.h> /* nanosleep (posix) */

#include "tinkervision/tinkervision.h"

int8_t start_module(){
	int8_t id = 0;
	int16_t result;

	result = tv_module_start("snapshot", &id);
		printf("Load module snapshot: result %d: %s\n", result,
				tv_result_string(result));

	return id;
}

void end_module(int8_t id ){
	int16_t result;

	result = tv_module_remove(id);
	printf("Remove module: %d (%s)\n", result,
			tv_result_string(result));
}


int main(int argc, char* argv[]) {
    uint16_t width, height;
    int16_t result;

    start_module();
    sleep(1);

    width = height = 0;
    result = tv_get_framesize(&width, &height);
    printf("Pre-Set GetResolution: %d (%s)\n", result, tv_result_string(result));
    printf("WxH: %lux%lu\n", (long unsigned)width, (long unsigned)height);
    sleep(1);

	width = 1280; height = 720;
	result = tv_set_framesize(width, height);
    printf("SetFramesize: %d (%s)\n", result, tv_result_string(result));
    sleep(2);

	/*
 	width = height = 0;
    result = tv_get_framesize(&width, &height);
    printf("Post-Set GetResolution: %d (%s)\n", result, tv_result_string(result));
    printf("WxH: %lux%lu\n", (long unsigned)width, (long unsigned)height);
    sleep(2);
	*/

    // end_module(id);

	return (0);
}
