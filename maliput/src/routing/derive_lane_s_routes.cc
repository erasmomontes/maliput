#include "maliput/routing/derive_lane_s_routes.h"

#include <optional>

#include "maliput/api/branch_point.h"
#include "maliput/api/lane.h"
#include "maliput/common/maliput_abort.h"
#include "maliput/routing/find_lane_sequences.h"

namespace maliput {
namespace routing {
namespace {

// Returns true iff @p lane is in @p set.
bool LaneExistsInSet(const api::LaneEndSet* set, const api::Lane* lane) {
  MALIPUT_DEMAND(set != nullptr);
  for (int i = 0; i < set->size(); ++i) {
    if (set->get(i).lane == lane) {
      return true;
    }
  }
  return false;
}

// Returns the S coordinate in @p lane that is on the border with @p next_lane.
std::optional<double> DetermineEdgeS(const api::Lane* lane, const api::Lane* next_lane) {
  MALIPUT_DEMAND(lane != nullptr);
  MALIPUT_DEMAND(next_lane != nullptr);
  if (LaneExistsInSet(lane->GetOngoingBranches(api::LaneEnd::kFinish), next_lane)) {
    return lane->length();
  }
  if (LaneExistsInSet(lane->GetOngoingBranches(api::LaneEnd::kStart), next_lane)) {
    return 0.;
  }
  return std::nullopt;
}

}  // namespace

std::vector<api::LaneSRoute> DeriveLaneSRoutes(const api::RoadPosition& start, const api::RoadPosition& end,
                                               double max_length_m) {
  MALIPUT_DEMAND(start.lane != nullptr);
  MALIPUT_DEMAND(end.lane != nullptr);
  const double start_s = start.pos.s();
  const double end_s = end.pos.s();
  std::vector<api::LaneSRoute> result;

  for (const auto& lane_sequence : FindLaneSequences(start.lane, end.lane, max_length_m)) {
    MALIPUT_DEMAND(!lane_sequence.empty());
    std::vector<api::LaneSRange> ranges;

    // Handles the case when lane_sequence has a length of 1. This occurs when
    // start and end are in the same lane.
    if (lane_sequence.size() == 1) {
      MALIPUT_DEMAND(start.lane == end.lane);
      const std::vector<api::LaneSRange> lane_s_ranges = {
          api::LaneSRange(start.lane->id(), api::SRange(start_s, end_s))};
      result.emplace_back(lane_s_ranges);
      continue;
    }

    // Handles the case when lane_sequence has a length greater than 1.
    for (size_t i = 0; i < lane_sequence.size(); ++i) {
      const api::Lane* lane = lane_sequence.at(i);
      MALIPUT_DEMAND(lane != nullptr);
      if (i == 0) {
        MALIPUT_DEMAND(lane->id() == start.lane->id());
        const std::optional<double> first_end_s = DetermineEdgeS(lane, lane_sequence.at(1));
        MALIPUT_DEMAND(first_end_s.has_value());
        ranges.emplace_back(lane->id(), api::SRange(start_s, first_end_s.value()));
      } else if (i + 1 == lane_sequence.size()) {
        MALIPUT_DEMAND(lane->id() == end.lane->id());
        MALIPUT_DEMAND(i > 0);
        const std::optional<double> last_start_s = DetermineEdgeS(lane, lane_sequence.at(i - 1));
        MALIPUT_DEMAND(last_start_s.has_value());
        ranges.emplace_back(lane->id(), api::SRange(last_start_s.value(), end_s));
      } else {
        const std::optional<double> middle_start_s = DetermineEdgeS(lane, lane_sequence.at(i - 1));
        const std::optional<double> middle_end_s = DetermineEdgeS(lane, lane_sequence.at(i + 1));
        MALIPUT_DEMAND(middle_start_s && middle_end_s);
        ranges.emplace_back(lane->id(), api::SRange(middle_start_s.value(), middle_end_s.value()));
      }
    }
    result.emplace_back(ranges);
  }
  return result;
}
}  // namespace routing
}  // namespace maliput
