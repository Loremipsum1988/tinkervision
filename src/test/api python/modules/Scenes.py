import unittest

from TvErrorCodes import TVE
from Util import Helper

##### Scenes ########################################################################################################
class Scenes(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.name = "Scenes"

        cls.mod_name1 = "downscale"
        cls.mod_name2 = "snapshot"

        cls.module1 = Helper.start_module(cls.mod_name1)
        cls.module2 = Helper.start_module(cls.mod_name2)

        print "[" + cls.name + "] Starting Tests."
        Helper.show_running_modules(cls.name)

    @classmethod
    def tearDownClass(cls):
        Helper.kill_all_modules()
        Helper.show_running_modules(cls.name)
        print "[" + cls.name + "] Tests finished."

    ###### 1. Check if Module was successfully created (module.result==0) ##############################################
    def test_created(self):
        self.assertEqual(self.module1.result, TVE.TV_OK)
        self.assertEqual(self.module2.result, TVE.TV_OK)

    def test_start_add_remove(self):
        self.vis = Helper.red.vision_scene_start(self.module1.id)
        self.assertEqual(self.vis.result, TVE.TV_OK)
        self.assertEqual(Helper.red.vision_scene_add(self.vis.scene_id, self.module2.id), TVE.TV_OK)
        self.assertEqual(Helper.red.vision_scene_remove(self.vis.scene_id), TVE.TV_OK)



