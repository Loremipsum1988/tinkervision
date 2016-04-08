import unittest

from Util import Helper
from bindings.brick_red import RED # generated from tinkervision generators!
from tinkerforge.ip_connection import IPConnection

##### TestModule Functions


def setUpModule():
    print "Setting up the Tests..."
    Helper.ipcon = IPConnection()
    Helper.ipcon.connect(Helper.HOST, Helper.PORT)
    Helper.red = RED(Helper.UID, Helper.ipcon)

    # Helper.red.register_callback(RED.CALLBACK_VISION_MODULE, Helper.vision_callback)

def tearDownModule():
    print "Tearing down..."
    Helper.ipcon.disconnect()



# FIXME no solution! just a workaround!
setUpModule()
# from modules.Colormatch import Colormatch
# from modules.Downscale import Downscale
# from modules.Functions import Functions
# from modules.Grayfilter import Grayfilter
# from modules.Module import Module
# from modules.Motiondetect import Motiondetect
from modules.Scenes import Scenes
# from modules.Snapshot import Snapshot
# from modules.Stream import Stream
# tearDownModule() # TODO how to export test classes?!

##### MAIN
if __name__ == '__main__':
    unittest.main()
