#include "safety_guard.hpp"

#include <cctype>
#include <limits>

namespace fan800 {
namespace {

int schema_major(const std::string &version)
{
    if (version.empty()) return -1;

    std::uint64_t major = 0;
    std::size_t index = 0;
    while (index < version.size() && std::isdigit(static_cast<unsigned char>(version[index]))) {
        major = major * 10 + static_cast<unsigned>(version[index] - '0');
        if (major > static_cast<std::uint64_t>(std::numeric_limits<int>::max())) return -2;
        ++index;
    }

    if (index == 0 || index == version.size() || version[index] != '.') return -2;
    return static_cast<int>(major);
}

bool event_requires_midi(const std::string &event)
{
    return event.rfind("MIDI_", 0) == 0;
}

} // namespace

SafetyGuard::SafetyGuard(std::unordered_set<std::string> supported_events,
                         bool allow_legacy_unversioned)
    : supported_events_(std::move(supported_events)),
      allow_legacy_unversioned_(allow_legacy_unversioned)
{
}

Decision SafetyGuard::evaluate(const EventRequest &request,
                               const SafetyState &state) const
{
    const int major = schema_major(request.event_schema_version);
    if (major == -1 && !allow_legacy_unversioned_)
        return {false, RejectReason::LegacySchemaDisabled};
    if (major != -1 && major != kSupportedEventSchemaMajor)
        return {false, RejectReason::UnsupportedSchema};

    if (!is_valid_event_name(request.event))
        return {false, RejectReason::InvalidEventName};
    if (supported_events_.find(request.event) == supported_events_.end())
        return {false, RejectReason::UnknownEvent};
    if (request.timestamp_ms < state.cooldown_until_ms)
        return {false, RejectReason::Cooldown};
    if (!state.temperature_ok)
        return {false, RejectReason::Temperature};
    if (!state.current_ok)
        return {false, RejectReason::Overcurrent};
    if (event_requires_midi(request.event) && !state.midi_connected)
        return {false, RejectReason::MidiNotConnected};

    return {true, RejectReason::None};
}

bool is_valid_event_name(const std::string &event)
{
    if (event.empty() || event.front() == '_' || event.back() == '_') return false;

    bool previous_underscore = false;
    for (const unsigned char c : event) {
        const bool underscore = c == '_';
        if (!(std::isupper(c) || std::isdigit(c) || underscore)) return false;
        if (underscore && previous_underscore) return false;
        previous_underscore = underscore;
    }
    return true;
}

const char *reject_reason_string(RejectReason reason)
{
    switch (reason) {
    case RejectReason::None: return "";
    case RejectReason::UnsupportedSchema: return "unsupported_schema";
    case RejectReason::LegacySchemaDisabled: return "legacy_schema_disabled";
    case RejectReason::InvalidEventName: return "invalid_event_name";
    case RejectReason::UnknownEvent: return "unknown_event";
    case RejectReason::Cooldown: return "cooldown";
    case RejectReason::Temperature: return "temperature";
    case RejectReason::Overcurrent: return "overcurrent";
    case RejectReason::MidiNotConnected: return "midi_not_connected";
    }
    return "unknown_reject_reason";
}

} // namespace fan800
