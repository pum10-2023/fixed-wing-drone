#include "mode.h"
#include "Plane.h"
#include "Parameters.h"

bool ModeLandBallistic::_enter() {
   
    plane.do_loiter_at_location();

    plane.next_WP_loc.set_alt_cm(plane.g2.ballistic_target_alt_cm, Location::AltFrame::ABOVE_TERRAIN);
    return true;
}

void ModeLandBallistic::update() {
    
    if (plane.adjusted_relative_altitude_cm() >= plane.g2.ballistic_target_alt_cm - 500
     && plane.adjusted_relative_altitude_cm() < plane.g2.ballistic_target_alt_cm) {
        nose_dive = true;
    } 

    if(nose_dive) {
        if (plane.nav_pitch_cd != -9000){
            plane.nav_pitch_cd = -9000;
            plane.nav_roll_cd = 0;
        } 
        SRV_Channels::set_output_scaled(SRV_Channel::k_throttle, -31.125);
    } else {
        plane.calc_nav_pitch();
        plane.calc_nav_roll();
        plane.calc_throttle();
    }
}

void ModeLandBallistic::navigate() {
    if (!nose_dive) {
        plane.update_loiter(8);
    }
}

bool ModeLandBallistic::handle_guided_request(Location target_loc) { return false; }
