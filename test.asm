.data ; data section
hello: .ascii "Hello World" ; define ascii string

.text ; text section
.global main ; mark main as entry point

main:
li $v0, -0xFF ; syscall 1
la $a0, hello ; syscall argument 1
syscall
; exit
jr $ra