#include "maliput/base/intersection.h"

#include "maliput/common/maliput_throw.h"

namespace maliput {

Intersection::Intersection(const Id& id,
                           const std::vector<api::rules::LaneSRange>& region,
                           const api::rules::PhaseRing& ring,
                           ManualPhaseProvider* phase_provider)
    : api::Intersection(id, region, ring), phase_provider_(phase_provider) {
  MALIPUT_THROW_UNLESS(phase_provider_ != nullptr);
}

drake::optional<api::rules::PhaseProvider::Result> Intersection::Phase() const {
  return phase_provider_->GetPhase(ring_id());
}

void Intersection::SetPhase(const api::rules::Phase::Id& phase_id) {
  phase_provider_->SetPhase(ring_id(), phase_id);
}

}  // namespace maliput
