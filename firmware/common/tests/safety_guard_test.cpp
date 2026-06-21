#include "safety_guard.hpp"

#include <cassert>
#include <iostream>
#include <unordered_set>

using fan800::EventRequest;
using fan800::RejectReason;
using fan800::SafetyGuard;
using fan800::SafetyState;

int main()
{
    SafetyGuard guard({"LIGHT_ON", "FIRE_SLIME_MEDIUM", "MIDI_NOTE_OUT"});
    SafetyState safe;

    assert(guard.evaluate({"1.0.0", "LIGHT_ON", 1000}, safe).approved);
    assert(guard.evaluate({"1.9.0", "LIGHT_ON", 1000}, safe).approved);
    assert(guard.evaluate({"", "LIGHT_ON", 1000}, safe).reason ==
           RejectReason::LegacySchemaDisabled);
    assert(guard.evaluate({"2.0.0", "LIGHT_ON", 1000}, safe).reason ==
           RejectReason::UnsupportedSchema);
    assert(guard.evaluate({"invalid", "LIGHT_ON", 1000}, safe).reason ==
           RejectReason::UnsupportedSchema);
    assert(guard.evaluate({"1.0.0", "GPIO_PIN_4_HIGH_500ms", 1000}, safe).reason ==
           RejectReason::InvalidEventName);
    assert(guard.evaluate({"1.0.0", "LIGHT_UNKNOWN", 1000}, safe).reason ==
           RejectReason::UnknownEvent);

    SafetyState cooldown = safe;
    cooldown.cooldown_until_ms = 1001;
    assert(guard.evaluate({"1.0.0", "FIRE_SLIME_MEDIUM", 1000}, cooldown).reason ==
           RejectReason::Cooldown);

    SafetyState hot = safe;
    hot.temperature_ok = false;
    assert(guard.evaluate({"1.0.0", "LIGHT_ON", 1000}, hot).reason ==
           RejectReason::Temperature);

    SafetyState overcurrent = safe;
    overcurrent.current_ok = false;
    assert(guard.evaluate({"1.0.0", "LIGHT_ON", 1000}, overcurrent).reason ==
           RejectReason::Overcurrent);

    SafetyState midi_missing = safe;
    midi_missing.midi_connected = false;
    assert(guard.evaluate({"1.0.0", "MIDI_NOTE_OUT", 1000}, midi_missing).reason ==
           RejectReason::MidiNotConnected);

    SafetyGuard legacy_guard({"LIGHT_ON"}, true);
    assert(legacy_guard.evaluate({"", "LIGHT_ON", 1000}, safe).approved);

    assert(std::string(fan800::reject_reason_string(RejectReason::Cooldown)) == "cooldown");
    std::cout << "safety_guard tests passed\n";
}
