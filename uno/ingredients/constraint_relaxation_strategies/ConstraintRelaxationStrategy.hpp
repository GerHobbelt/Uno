// Copyright (c) 2018-2024 Charlie Vanaret
// Licensed under the MIT license. See LICENSE file in the project directory for details.

#ifndef UNO_CONSTRAINTRELAXATIONSTRATEGY_H
#define UNO_CONSTRAINTRELAXATIONSTRATEGY_H

#include <cstddef>
#include <functional>
#include <memory>
#include "linear_algebra/Norm.hpp"
#include "optimization/IterateStatus.hpp"

namespace uno {
   // forward declarations
   class Direction;
   class GlobalizationStrategy;
   class HessianModel;
   class Iterate;
   template <typename ElementType>
   class LagrangianGradient;
   class Model;
   class Multipliers;
   class OptimizationProblem;
   class Options;
   class Statistics;
   class InequalityHandlingMethod;
   template <typename IndexType, typename ElementType>
   class SymmetricMatrix;
   class UserCallbacks;
   template <typename ElementType>
   class Vector;
   struct WarmstartInformation;

   class ConstraintRelaxationStrategy {
   public:
      explicit ConstraintRelaxationStrategy(const Options& options);
      virtual ~ConstraintRelaxationStrategy();

      virtual void initialize(Statistics& statistics, const Model& model, Iterate& initial_iterate, Direction& direction, const Options& options) = 0;
      void set_trust_region_radius(double trust_region_radius);

      // direction computation
      virtual void compute_feasible_direction(Statistics& statistics, const Model& model, Iterate& current_iterate, Direction& direction,
         WarmstartInformation& warmstart_information) = 0;
      void compute_feasible_direction(Statistics& statistics, const Model& model, Iterate& current_iterate, Direction& direction,
         const Vector<double>& initial_point, WarmstartInformation& warmstart_information);
      [[nodiscard]] virtual bool solving_feasibility_problem() const = 0;
      virtual void switch_to_feasibility_problem(Statistics& statistics, const Model& model, Iterate& current_iterate,
         WarmstartInformation& warmstart_information) = 0;

      // trial iterate acceptance
      [[nodiscard]] virtual bool is_iterate_acceptable(Statistics& statistics, const Model& model, Iterate& current_iterate, Iterate& trial_iterate,
         const Direction& direction, double step_length, WarmstartInformation& warmstart_information, UserCallbacks& user_callbacks) = 0;
      [[nodiscard]] IterateStatus check_termination(const Model& model, Iterate& iterate);

      // primal-dual residuals
      virtual void compute_primal_dual_residuals(const Model& model, Iterate& iterate) = 0;
      virtual void set_dual_residuals_statistics(Statistics& statistics, const Iterate& iterate) const = 0;

      [[nodiscard]] virtual size_t get_hessian_evaluation_count() const = 0;
      [[nodiscard]] size_t get_number_subproblems_solved() const;

   protected:
      const std::unique_ptr<GlobalizationStrategy> globalization_strategy;
      const std::unique_ptr<InequalityHandlingMethod> inequality_handling_method;
      const Norm progress_norm;
      const Norm residual_norm;
      const double residual_scaling_threshold;
      const double tight_tolerance; /*!< Tight tolerance of the termination criteria */
      const double loose_tolerance; /*!< Loose tolerance of the termination criteria */
      size_t loose_tolerance_consecutive_iterations{0};
      const size_t loose_tolerance_consecutive_iteration_threshold;
      const double unbounded_objective_threshold;
      // first_order_predicted_reduction is true when the predicted reduction can be taken as first-order (e.g. in line-search methods)
      const bool first_order_predicted_reduction;

      void set_objective_measure(const Model& model, Iterate& iterate) const;
      void set_infeasibility_measure(const Model& model, Iterate& iterate) const;
      [[nodiscard]] double compute_predicted_infeasibility_reduction_model(const Model& model, const Iterate& current_iterate,
         const Vector<double>& primal_direction, double step_length) const;
      [[nodiscard]] std::function<double(double)> compute_predicted_objective_reduction_model(const OptimizationProblem& problem,
         HessianModel& hessian_model, const Iterate& current_iterate, const Multipliers& multipliers, const Vector<double>& primal_direction,
         double step_length) const;
      void compute_progress_measures(const Model& model, Iterate& current_iterate, Iterate& trial_iterate);
      virtual void evaluate_progress_measures(const Model& model, Iterate& iterate) const = 0;

      void compute_primal_dual_residuals(const Model& model, const OptimizationProblem& optimality_problem,
         const OptimizationProblem& feasibility_problem, Iterate& iterate);

      [[nodiscard]] double compute_stationarity_scaling(const Model& model, const Multipliers& multipliers) const;
      [[nodiscard]] double compute_complementarity_scaling(const Model& model, const Multipliers& multipliers) const;

      [[nodiscard]] IterateStatus check_first_order_convergence(const Model& model, Iterate& current_iterate, double tolerance) const;

      void set_statistics(Statistics& statistics, const Model& model, const Iterate& iterate) const;
      void set_progress_statistics(Statistics& statistics, const Model& model, const Iterate& iterate) const;
   };
} // namespace

#endif //UNO_CONSTRAINTRELAXATIONSTRATEGY_H