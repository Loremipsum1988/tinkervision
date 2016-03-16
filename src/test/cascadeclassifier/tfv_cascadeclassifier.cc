/*
 Tinkervision - Vision Library for https://github.com/Tinkerforge/red-brick
 Copyright (C) 2016 roland.dudko@fh-bielefeld.de

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

#include "tinkervision/tinkervision.h"

int8_t id = 0;

void callback(int8_t id, TV_ModuleResult result, void* context) {
	printf("Cascadeclassifier: Modul-ID:%d --- X:%d Y:%d String:%s \n", id,
			result.x, result.y, result.string);
}

void test_without_grayscale() {
	printf("test_without_grayscale\n");

	int16_t result = tv_module_set_numerical_parameter(id, "use_grayscale", 0);
	printf("Set use_grayscale: TN-2 Code %d (%s)\n", result,
			tv_result_string(result));

	sleep(1);
}

void test_switch_path() {
	printf("test_switch_path\n");

	int16_t result = tv_module_set_string_parameter(id, "path_to_model",
			"/home/tf/tv/lib/model/test/");
	printf("Set path_to_model: Code %d (%s)\n", result,
			tv_result_string(result));

	sleep(2);
}

void test_single_xmlfile() {
	printf("test_single_xmlfile\n");

	int16_t result = tv_module_set_string_parameter(id, "path_to_model",
			"/home/tf/tv/lib/model/haarcascade_frontalface_default.xml");
	printf("Set path_to_model: Code %d (%s)\n", result,
			tv_result_string(result));

	sleep(2);
}

int main(int argc, char* argv[]) {
	int16_t result = TV_INTERNAL_ERROR;

	printf("Started Cascadeclassifier\n");
	result = tv_camera_available();
	if (result != 0) {
		printf("Requested camera not available: %s\n",
				tv_result_string(result));
		return (-1);
	}
	sleep(1);

	result = tv_module_start("cascadeclassifier", &id);
	printf("Configured module id %d: Code %d (%s)\n", id, result,
			tv_result_string(result));

	result = tv_callback_enable_default(callback);
	printf("Set callback: Code %d (%s)\n", result, tv_result_string(result));
	sleep(1);

	test_without_grayscale();
	test_switch_path();
	test_single_xmlfile();

	/* Remove module
	 result = tv_module_remove(id);
	 printf("Removed module %d: Code %d (%s)\n", id, result,
	 tv_result_string(result));
	 */
	sleep(2);
	printf("Finished Cascadeclassifier\n");

	return (0);
}
