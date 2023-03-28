#include "mode.h"
#include "Plane.h"
#include "Parameters.h"

bool ModeLandBallistic::_enter() {
    plane.do_loiter_at_location();

    plane.next_WP_loc.set_alt_cm(plane.g2.ballistic_target_alt_cm, Location::AltFrame::ABOVE_TERRAIN);
    return true;
}

void ModeLandBallistic::update() {
    plane.calc_nav_roll();
    plane.calc_nav_pitch();
    plane.calc_throttle();
}

void ModeLandBallistic::navigate() {

    plane.update_loiter(0);
}


bool ModeLandBallistic::handle_guided_request(Location target_loc) { return false; }
