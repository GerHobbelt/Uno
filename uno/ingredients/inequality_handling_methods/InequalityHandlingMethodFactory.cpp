// Copyright (c) 2018-2024 Charlie Vanaret
// Licensed under the MIT license. See LICENSE file in the project directory for details.

#include <string>
#include "InequalityHandlingMethod.hpp"
#include "InequalityHandlingMethodFactory.hpp"
#include "inequality_constrained_methods/QPSubproblem.hpp"
#include "inequality_constrained_methods/LPSubproblem.hpp"
#include "interior_point_methods/PrimalDualInteriorPointMethod.hpp"
#include "ingredients/subproblem_solvers/QPSolverFactory.hpp"
#include "ingredients/subproblem_solvers/SymmetricIndefiniteLinearSolverFactory.hpp"
#include "options/Options.hpp"

namespace uno {
   std::unique_ptr<InequalityHandlingMethod> InequalityHandlingMethodFactory::create(const Options& options) {
      const std::string subproblem_strategy = options.get_string("subproblem");
      // inequality-constrained methods
      if (subproblem_strategy == "QP") {
         return std::make_unique<QPSubproblem>(options);
      }
      else if (subproblem_strategy == "LP") {
         return std::make_unique<LPSubproblem>(options);
      }
      // interior-point method
      else if (subproblem_strategy == "primal_dual_interior_point") {
         return std::make_unique<PrimalDualInteriorPointMethod>(options);
      }
      throw std::invalid_argument("Subproblem strategy " + subproblem_strategy + " is not supported");
   }

   std::vector<std::string> InequalityHandlingMethodFactory::available_strategies() {
      std::vector<std::string> strategies{};
      if (!QPSolverFactory::available_solvers().empty()) {
         strategies.emplace_back("QP");
         strategies.emplace_back("LP");
      }
      if (!SymmetricIndefiniteLinearSolverFactory::available_solvers().empty()) {
         strategies.emplace_back("primal_dual_interior_point");
      }
      return strategies;
   }
} // namespace