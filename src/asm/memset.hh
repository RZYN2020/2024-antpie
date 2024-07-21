// Sets the first @var{count} bytes of @var{s} to the constant byte
// @var{c}, returning a pointer to @var{s}.
// void memset (void *@var{s}, int @var{c}, @int @var{count})
// IR type : (PointerType, IntType, IntType) -> VoidType
#include <string>

std::string memset_code = "memset:\n"
                          "        addi    sp,sp,-48\n"
                          "        sd      ra,40(sp)\n"
                          "        sd      s0,32(sp)\n"
                          "        sd      s1,24(sp)\n"
                          "        addi    s0,sp,48\n"
                          "        sd      a0,-40(s0)\n"
                          "        mv      a3,a2\n"
                          "        ld      s1,-40(s0)\n"
                          "        j       .L2\n"
                          ".L3:\n"
                          "        andi    a4,a1,0xff\n"
                          "        mv      a5,s1\n"
                          "        addi    s1,a5,1\n"
                          "        sb      a4,0(a5)\n"
                          ".L2:\n"
                          "        mv      a5,a3\n"
                          "        addiw   a4,a5,-1\n"
                          "        sext.w  a3,a4\n"
                          "        sgt     a5,a5,zero\n"
                          "        andi    a5,a5,0xff\n"
                          "        bne     a5,zero,.L3\n"
                          "        ld      a5,-40(s0)\n"
                          "        mv      a0,a5\n"
                          "        ld      ra,40(sp)\n"
                          "        ld      s0,32(sp)\n"
                          "        ld      s1,24(sp)\n"
                          "        addi    sp,sp,48\n"
                          "        jr      ra\n";