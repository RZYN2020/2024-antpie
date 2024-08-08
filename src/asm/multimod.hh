// int multimod(int x, int y, int m)

#include <string>

std::string multimod_code =
    "antpie_multimod:\n"
    "        mul    a3, a1, a0\n"
    "        fcvt.d.l        fa5, a0\n"
    "        fcvt.d.l        fa4, a1\n"
    "        fmul.d  fa5, fa5, fa4\n"
    "        fcvt.d.l        fa4, a2\n"
    "        fdiv.d  fa5, fa5, fa4\n"
    "        fcvt.l.d        a0, fa5, rtz\n"
    "        mul     a0, a0, a2\n"
    "        sub     a3, a3, a0\n"
    "        rem     a0, a3, a2\n"
    "        srai    a1, a0, 63\n"
    "        and     a1, a1, a2\n"
    "        addw    a0, a0, a1\n"
    "        ret\n";