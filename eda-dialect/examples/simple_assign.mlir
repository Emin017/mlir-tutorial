module {
  %0 = "eda.constant"() {value = "10"} : () -> !eda.string
  %1 = "eda.assign"(%0) {name = "timeout"} : (!eda.string) -> !eda.string
  "eda.puts"(%1) : (!eda.string) -> ()
}
