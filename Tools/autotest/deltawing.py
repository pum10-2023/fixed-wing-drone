import os
import pexpect

from arduplane import AutoTestPlane
from common import AutoTest, NotAchievedException

testdir = os.path.dirname(os.path.realpath(__file__))


class AutoTestDeltaWing(AutoTestPlane):
    def log_name(self):
        return "DeltaWing"

    def test_filepath(self):
        return os.path.realpath(__file__)

    def defaults_filepath(self):
        return os.path.join(testdir, 'default_params/gazebo-zephyr.parm')

    def set_current_test_name(self, name):
        # Potentialy just use the same as regular plane
        self.current_test_name_directory = "DeltaWing_Tests/" + name + "/"

    def default_frame(self):
        return "gazebo-zephyr"

    def start_gazebo(self):
        self.progress("Starting Gazebo")
        self.gazebo = pexpect.spawn('gz sim -r -s zephyr_runway.sdf')

    def stop_gazebo(self):
        self.progress("Stopping Gazebo")
        if self.gazebo is None or not self.gazebo.isalive():
            raise ValueError("Gazebo is not running")
        self.gazebo.close(force = True)

    def start_SITL(self, binary=None, **sitl_args):
        # Use gazebo simulator as backend.
        self.start_gazebo()
        sitl_args["speedup"] = '1'
        if "model" in sitl_args:
            if sitl_args["model"] != "JSON":
                raise ValueError("DeltaWing model must be JSON")
        else:
            sitl_args["model"] = 'JSON'
        sitl_args["customisations"] = ["--sim-address=127.0.0.1"]
        super(AutoTestDeltaWing, self).start_SITL(binary = binary, **sitl_args)

    def stop_SITL(self):
        self.stop_gazebo()
        super(AutoTestDeltaWing, self).stop_SITL()

    def reboot_sitl(self, required_bootcount=None, force=False):
        self.stop_gazebo()
        self.start_gazebo()
        super(AutoTestDeltaWing, self).reboot_sitl(required_bootcount=required_bootcount, force=force)

    def takeoff(self, alt=150, alt_max = None, relative = True):
        if alt_max is None:
            alt_max = alt + 30
        
        self.change_mode("FBWA")

        self.wait_ready_to_arm()
        self.arm_vehicle()

        # Takeoff from standing position
        self.set_rc(2, 1800)
        self.set_rc(3, 1800)

        # Reach target altitude
        self.wait_altitude(alt, alt_max, timeout=40, relative=relative)

        # Set pitch to neutral
        self.set_rc(2, 1500)

        self.progress("TAKEOFF COMPLETE")

    def BallisticLanding(self):
        '''test ballistic landing mode'''
        self.takeoff(alt = 100)
        
        self.start_subtest("Set BLAND_START_ALT parameter")
        target_altitude = 15
        self.set_parameter('BLAND_START_ALT', target_altitude * 100)

        self.start_subtest("Change mode to LAND_BALLISTIC")
        self.change_mode('LAND_BALLISTIC')
        loc = self.mav.location()
        self.start_subtest("Circle down to altitude 15")
        self.wait_altitude(target_altitude, target_altitude + 1, timeout=50, relative=True)
        here = self.mav.location()
        dist = self.get_distance(loc, here)
        self.progress("Distance traveled=%.1f" % dist)
        if dist > 20:
            raise NotAchievedException("Moved to far from initial position")

        self.disarm_vehicle(force=True)

    def tests(self):
        '''return list of all tests'''
        return [
            self.BallisticLanding
        ]
