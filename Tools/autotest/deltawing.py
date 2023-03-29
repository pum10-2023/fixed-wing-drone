import os

from arduplane import AutoTestPlane
from common import AutoTest

testdir = os.path.dirname(os.path.realpath(__file__))

class AutoTestDeltaWing(AutoTestPlane):
    def log_name(self):
        return "DeltaWing"

    def test_filepath(self):
        return os.path.realpath(__file__)

    def defaults_filepath(self):
        # Probably change later
        return os.path.join(testdir, 'default_params/plane-jsbsim.parm')

    def set_current_test_name(self, name):
        # Potemtialy just use the same as regular plane
        self.current_test_name_directory = "DeltaWing_Tests/" + name + "/"

    def default_frame(self):
        return "gazebo-zephyr"

    def takeoff(self, alt=150, alt_max = None, relative = True):
        if alt_max is None:
            alt_max = alt + 30
        
        self.set_mode("FBWA")

        self.wait_ready_to_arm()
        self.arm_vehicle()

        # Takeoff from standing position
        self.set_rc(2, 1800)
        self.set_rc(3, 1800)

        # Reach target altitude
        self.wait_altitude(alt, alt_max, timeout=30, relative=relative)

        # Set pitch to neutral
        self.set_rc(2, 1500)

        self.progress("TAKEOFF COMPLETE")

    def BallisticLanding(self):
        '''test ballistic landing mode'''
        target_altitude = 1500
        self.set_parameter('BLAND_START_ALT', target_altitude)
        self.change_mode('LAND_BALLISTIC')
        self.wait_altitude(target_altitude - 5, target_altitude + 5, timeout=80)


    def tests(self):
        '''return list of all tests'''
        ret = AutoTest.tests(self)
        ret.extend(
            [
                self.BallisticLanding
            ]
        )
        return ret

    