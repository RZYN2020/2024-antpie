.section .data
.text
.globl main
defn:
.defn.defn_entry0:
	j .defn.entry0

.defn.entry0:
	ret 4


main:
.main.main_entry0:
	a.addr0 = alloca 4
	j .main.entry1

.main.entry1:
	calldefn0 = call defn()
	sw calldefn0, 0(a.addr0)
	lw get0, 0(a.addr0)
	ret get0


