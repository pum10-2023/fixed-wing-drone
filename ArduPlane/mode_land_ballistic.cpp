#include "mode.h"
#include "Plane.h"

bool ModeLandBallistic::_enter() {
    plane.do_loiter_at_location();
    plane.next_WP_loc.set_alt_cm(500, Location::AltFrame::ABOVE_TERRAIN);
    return true;
}

void ModeLandBallistic::update() {
    plane.calc_nav_roll();
    plane.calc_nav_pitch();
    plane.calc_throttle();
}

void ModeLandBallistic::navigate() {

    plane.update_loiter(8);
}


bool ModeLandBallistic::handle_guided_request(Location target_loc) { return false; }
