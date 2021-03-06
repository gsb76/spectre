// Distributed under the MIT License.
// See LICENSE.txt for details.

#define CATCH_CONFIG_RUNNER

#include <vector>

#include "ErrorHandling/FloatingPointExceptions.hpp"
#include "NumericalAlgorithms/LinearSolver/ConjugateGradient/ConjugateGradient.hpp"
#include "Parallel/ConstGlobalCache.hpp"
#include "Parallel/InitializationFunctions.hpp"
#include "Parallel/Main.hpp"
#include "Utilities/TMPL.hpp"
#include "tests/Unit/NumericalAlgorithms/LinearSolver/DistributedLinearSolverAlgorithmTestHelpers.hpp"

namespace {

struct Metavariables {
  using system = DistributedLinearSolverAlgorithmTest::System;

  using linear_solver = LinearSolver::ConjugateGradient<Metavariables>;

  using component_list = tmpl::append<
      tmpl::list<
          DistributedLinearSolverAlgorithmTest::ElementArray<Metavariables>>,
      typename linear_solver::component_list>;
  using const_global_cache_tag_list = tmpl::list<>;

  static constexpr const char* const help{
      "Test the conjugate gradient linear solver algorithm on multiple "
      "elements"};
  static constexpr bool ignore_unrecognized_command_line_options = false;

  enum class Phase { Initialization, PerformLinearSolve, TestResult, Exit };

  static Phase determine_next_phase(
      const Phase& current_phase,
      const Parallel::CProxy_ConstGlobalCache<
          Metavariables>& /*cache_proxy*/) noexcept {
    switch (current_phase) {
      case Phase::Initialization:
        return Phase::PerformLinearSolve;
      case Phase::PerformLinearSolve:
        return Phase::TestResult;
      default:
        return Phase::Exit;
    }
  }
};

}  // namespace

static const std::vector<void (*)()> charm_init_node_funcs{
    &setup_error_handling};
static const std::vector<void (*)()> charm_init_proc_funcs{
    &enable_floating_point_exceptions};

using charmxx_main_component = Parallel::Main<Metavariables>;

#include "Parallel/CharmMain.cpp"
