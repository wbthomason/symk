#include "validation_selector.h"

#include "../../option_parser.h"
#include "../../task_utils/task_properties.h"

namespace symbolic {

ValidationSelector::ValidationSelector(const options::Options &opts)
    : PlanDataBase(opts), original_task_proxy(*tasks::g_root_task),
      original_state_registry(original_task_proxy) {
  anytime_completness = true;
}

void ValidationSelector::add_plan(const Plan &plan) {
  if (!has_rejected_plan(plan) && !has_accepted_plan(plan)) {
    if (is_valid_plan(plan)) {
      save_accepted_plan(plan);
    } else {
      save_rejected_plan(plan);
      std::cout << "Rejected: " << num_rejected_plans << std::endl;
    }
  }
}

bool ValidationSelector::is_valid_plan(const Plan &plan) {
  GlobalState cur = sym_vars->get_state_registry()->get_initial_state();

  for (size_t i = 0; i < plan.size(); i++) {
    auto original_op_id =
        sym_vars->get_state_registry()
            ->get_task_proxy()
            .get_operators()[i]
            .get_ancestor_operator_id(tasks::g_root_task.get());
    auto original_op =
        sym_vars->get_state_registry()->get_task_proxy().get_operators()[i];
    // original_task_proxy.get_operators()[original_op_id];
    if (task_properties::is_applicable(original_op, cur.unpack()))
    {
      cur = sym_vars->get_state_registry()->get_successor_state(cur, original_op);
    }
    else {
      return false;
    }
  }
  return true;
}

static std::shared_ptr<PlanDataBase> _parse(OptionParser &parser) {
  PlanDataBase::add_options_to_parser(parser);

  Options opts = parser.parse();
  if (parser.dry_run())
    return nullptr;
  return std::make_shared<ValidationSelector>(opts);
}

static Plugin<PlanDataBase> _plugin("validation_selector", _parse);

} // namespace symbolic
