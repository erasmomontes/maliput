#pragma once

#include "drake/common/drake_copyable.h"
#include "drake/common/drake_optional.h"

#include "maliput/api/rules/discrete_value_rule.h"
#include "maliput/api/rules/rule.h"
#include "maliput/api/rules/state_provider_result.h"

namespace maliput {
namespace api {
namespace rules {

/// Abstract interface for the state provider of DiscreteValueRules.
class DiscreteValueRuleStateProvider {
 public:
  /// The state of a DiscreteValueRule, returned by
  /// DiscreteValueRuleStateProvider::GetState(const Rule::Id&).
  using StateResult = StateProviderResult<DiscreteValueRule::DiscreteValue>;

  DRAKE_NO_COPY_NO_MOVE_NO_ASSIGN(DiscreteValueRuleStateProvider)

  virtual ~DiscreteValueRuleStateProvider() = default;

  /// Gets the state of the DiscreteValueRule identified by `id`.
  ///
  /// Returns a StateResult struct bearing the state of the rule with the
  /// specified `id`. If `id` is unrecognized, drake::nullopt is returned.
  drake::optional<StateResult> GetState(const Rule::Id& id) const { return DoGetState(id); }

 protected:
  DiscreteValueRuleStateProvider() = default;

 private:
  virtual drake::optional<StateResult> DoGetState(const Rule::Id& id) const = 0;
};

}  // namespace rules
}  // namespace api
}  // namespace maliput
