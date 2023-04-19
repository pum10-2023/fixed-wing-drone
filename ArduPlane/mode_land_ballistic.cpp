#include "mode.h"
#include "Plane.h"
#include "Parameters.h"


bool ModeLandBallistic::_enter()
{

    plane.do_loiter_at_location();

    plane.next_WP_loc.set_alt_cm(plane.g2.ballistic_target_alt_cm, Location::AltFrame::ABOVE_TERRAIN);
    plane.set_flight_stage(AP_FixedWing::FlightStage::NORMAL);
    return true;
}

void ModeLandBallistic::update()
{

    const float alt = plane.adjusted_relative_altitude_cm();
    const float target_alt = plane.g2.ballistic_target_alt_cm.get();

    if (alt >= target_alt - 500 && alt < target_alt) {
        plane.set_flight_stage(AP_FixedWing::FlightStage::LAND_BALLISTIC);
    }

    if (plane.flight_stage == AP_FixedWing::FlightStage::LAND_BALLISTIC) {
        if (plane.nav_pitch_cd != -9000) {
            plane.nav_pitch_cd = -9000;
            plane.nav_roll_cd = 0;
        }

        const float thrust = ((target_alt - alt) / target_alt) * plane.g2.ballistic_reverse_thrust;
        SRV_Channels::set_output_scaled(SRV_Channel::k_throttle, -thrust);
    } else {
        plane.calc_nav_pitch();
        plane.calc_nav_roll();
        plane.calc_throttle();
    }
}

void ModeLandBallistic::navigate()
{
    if (plane.flight_stage != AP_FixedWing::FlightStage::LAND_BALLISTIC) {
        plane.update_loiter(8);
    }
}

bool ModeLandBallistic::handle_guided_request(Location target_loc) { return false; }
