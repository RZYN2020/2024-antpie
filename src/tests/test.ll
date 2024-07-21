
define dso_local i32 @defn() {
defn_entry0:
  br label %entry0

entry0:
  ret i32 4

}

define dso_local i32 @main() {
main_entry0:
  %a.addr0 = alloca i32
  br label %entry1

entry1:
  %calldefn0 = call i32 @defn()
  store i32 %calldefn0, i32* %a.addr0
  %get0 = load i32, i32* %a.addr0
  ret i32 %get0

}

