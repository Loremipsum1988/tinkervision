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
#include <unistd.h> /* sleep (posix) */
#include <time.h>   /* nanosleep (posix) */
#include <stdlib.h>
#include "tinkervision/tinkervision.h"

int16_t result, scene=0, scene2=1;
int8_t module_id = 0, module_id2 = 1;

void callback(int8_t id, TV_ModuleResult result, void* context) {
	printf("Executing ID: %d \n", id);
}

void test_node_handling() {
	/* Create modules*/
	result = tv_module_start("grayfilter", &module_id);
	tv_module_stop(module_id);

	printf("Grayfilter Id %d Start: %d (%s)\n", module_id, result,
			tv_result_string(result));

	result = tv_module_start("snapshot", &module_id2);
	tv_module_stop(module_id2);

	printf("Grayfilter Id %d Start: %d (%s)\n", module_id2, result,
			tv_result_string(result));

	sleep(2);


	/* Create scenetree */
	result = tv_scene_from_module(module_id, &scene);
	printf("Started scene from %d as %d: %s (%d)\n", module_id, scene,
			tv_result_string(result), result);
	result = tv_scene_from_module(module_id2, &scene2);
	printf("Started scene from %d as %d: %s (%d)\n", module_id, scene,
			tv_result_string(result), result);
	sleep(2);

    result = tv_scene_add_module(scene, module_id2);
    printf("Scening %d to %d...: %s (%d)\n", module_id2, scene,
           tv_result_string(result), result);

    tv_module_restart(module_id);
    tv_module_restart(module_id2);
	sleep(5);
}

void test_module_handling() {

}

int main(int argc, char* argv[]) {
	result = tv_callback_enable_default(callback);
	printf("Set callback: Code %d (%s)\n", result, tv_result_string(result));

	test_node_handling();
	test_module_handling();


	printf("SCENE_ID: %d , %d \n", scene, scene2);
	printf("Removing Scene \n");
	tv_scene_remove(scene);
	sleep(3);

	tv_quit();
	return (0);
}
