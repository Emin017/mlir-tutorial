// Optimization example for EDA Dialect
// This example demonstrates various optimization opportunities

module {
  // Constants that can be folded
  %const_10 = "eda.constant"(10) : i32
  %const_20 = "eda.constant"(20) : i32
  %const_5 = "eda.constant"(5) : i32
  %const_true = "eda.constant"(true) : i1
  %const_false = "eda.constant"(false) : i1

  // Variable assignments
  %timeout = "eda.assign"("timeout", %const_10) : (i32) -> i32
  %retry_count = "eda.assign"("retry_count", 0) : (i32) -> i32

  // Dead code - unused computation
  %unused_sum = "eda.add"(%const_10, %const_5) : (i32, i32) -> i32
  %unused_product = "eda.mul"(%const_20, %const_5) : (i32, i32) -> i32

  // Computations that can be constant folded
  %foldable_sum = "eda.add"(%const_10, %const_20) : (i32, i32) -> i32
  %foldable_product = "eda.mul"(%const_10, %const_5) : (i32, i32) -> i32

  // Constant comparisons that can be folded
  %always_true = "eda.cmp_eq"(%const_10, %const_10) : (i32, i32) -> i1
  %always_false = "eda.cmp_gt"(%const_10, %const_20) : (i32, i32) -> i1

  // Conditional that will be known after constant folding
  "eda.if"(%always_true) ({
    "eda.puts"("This will always execute") : () -> ()
    // More dead code inside the if
    %dead_inside = "eda.add"(%foldable_sum, %foldable_product) : (i32, i32) -> i32
  }, {
    "eda.puts"("This will never execute") : () -> ()
  }) : () -> ()

  // Another conditional with known false condition
  "eda.if"(%always_false) ({
    "eda.puts"("This will never execute either") : () -> ()
  }) : () -> ()

  // Use the folded values
  "eda.store"("max_retries", %foldable_sum) : (i32) -> ()
  "eda.puts"("Max retries set to 30") : () -> ()

  // Loop with known bounds (can be unrolled)
  %start = "eda.constant"(0) : i32
  %end = "eda.constant"(3) : i32
  "eda.for"(%start, %end) ({
  ^bb0(%i: i32):
    // This loop can be unrolled since bounds are known
    %msg = "eda.format"("Iteration %d", %i) : (!tcl_string, i32) -> !tcl_string
    "eda.puts"(%msg) : (!tcl_string) -> ()
    "eda.yield"() : () -> ()
  }) : () -> ()

  // Function definition with dead parameter
  "eda.func"(@dead_code_func, (i32) -> i32) {
  ^bb0(%unused_param: i32):
    %result = "eda.constant"(42) : i32
    "eda.return"(%result) : (i32) -> i32
  }

  // Call the function (but don't use the result)
  %func_result = "eda.call"(@dead_code_func, %const_5) : (i32) -> i32
  // func_result is dead - the call can be removed
}