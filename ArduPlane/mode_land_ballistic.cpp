#include "mode.h"
#include "Plane.h"

bool ModeLandBallistic::_enter() {
   
    plane.do_loiter_at_location();
    plane.next_WP_loc.set_alt_cm(10000, Location::AltFrame::ABOVE_TERRAIN);

   return true;
}

void ModeLandBallistic::update() {
    
    if (plane.adjusted_relative_altitude_cm() >= 9500 && plane.adjusted_relative_altitude_cm() < 10000){
        nose_dive = true;
    } 
    // TODO: försöka att hastigheten når 3 m/s och att den dyker med 90 grader. 
    // TODO: sätta upp live share 
    if(nose_dive){
        if (plane.nav_pitch_cd != -9000){
            plane.nav_pitch_cd = -9000 - plane.nav_pitch_cd;
            plane.nav_roll_cd = 0;
        } else {
            plane.nav_pitch_cd = 0;
            plane.nav_roll_cd = 0;
        }
    } else {
        plane.calc_nav_pitch();
        plane.calc_nav_roll();
    }
    //gcs().send_text(MAV_SEVERITY::MAV_SEVERITY_DEBUG, "altitude: %u", plane.adjusted_altitude_cm());
    plane.calc_throttle();
}

void ModeLandBallistic::navigate() {
    plane.update_loiter(8);
    if (nose_dive){
        // gcs().send_text(MAV_SEVERITY::MAV_SEVERITY_DEBUG, "Arrived at 100 meters");

        plane.target_airspeed_cm = 300;
        plane.update_fbwb_speed_height();
        
        float airspeed_estimate = 0.0;
        ahrs.airspeed_estimate(airspeed_estimate);
        if ((int32_t)airspeed_estimate == 300){
            
            
            
            gcs().send_text(MAV_SEVERITY::MAV_SEVERITY_DEBUG, "this is the airspeed: %f", airspeed_estimate);
            
        }
    }
    plane.calc_nav_roll();
}

bool ModeLandBallistic::handle_guided_request(Location target_loc) { return false; }
