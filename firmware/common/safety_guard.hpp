#pragma once

#include <cstdint>
#include <string>
#include <unordered_set>

namespace fan800 {

inline constexpr int kSupportedEventSchemaMajor = 1;
inline constexpr const char *kEventSchemaVersion = "1.0.0";

enum class RejectReason {
    None,
    UnsupportedSchema,
    LegacySchemaDisabled,
    InvalidEventName,
    UnknownEvent,
    Cooldown,
    Temperature,
    Overcurrent,
    MidiNotConnected,
};

struct SafetyState {
    bool temperature_ok = true;
    bool current_ok = true;
    bool midi_connected = true;
    std::uint64_t cooldown_until_ms = 0;
};

struct EventRequest {
    std::string event_schema_version;
    std::string event;
    std::uint64_t timestamp_ms = 0;
};

struct Decision {
    bool approved = false;
    RejectReason reason = RejectReason::None;
};

class SafetyGuard {
public:
    SafetyGuard(std::unordered_set<std::string> supported_events,
                bool allow_legacy_unversioned = false);

    Decision evaluate(const EventRequest &request,
                      const SafetyState &state) const;

private:
    std::unordered_set<std::string> supported_events_;
    bool allow_legacy_unversioned_;
};

bool is_valid_event_name(const std::string &event);
const char *reject_reason_string(RejectReason reason);

} // namespace fan800
