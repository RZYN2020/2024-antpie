#include "DomTree.hh"
#include "Machine.hh"
#include "Module.hh"
#include <algorithm>


///////// Macro Defs /////////
// capture (res)
#define ADD_INSTR(INSTR, CONS, ...)                                            \
  auto INSTR = new CONS(__VA_ARGS__);                                          \
  mbb->pushInstr(INSTR)

bool is_constant(Value *v) {
  return v->getValueTag() == VT_INTCONST || v->getValueTag() == VT_FLOATCONST;
}

void lowerHIicmp(MFunction *mfunc) {
  // 2.1 combine
  for (auto &mbb : mfunc->getBasicBlocks()) {
    auto jmp = mbb->getJmp(0);
    if (jmp->getInsTag() == MInstruction::H_BR) {
      auto br = static_cast<MHIbr *>(jmp);
      auto tb = br->getTBlock();
      auto fb = br->getFBlock();
      auto cond = br->getReg(0);
      if (cond->getTag() == Register::V_IREGISTER) {
        auto vreg = static_cast<VRegister *>(cond);
        if (vreg->isInstruction()) {
          auto condi = static_cast<MInstruction *>(cond);
          if (condi->getInsTag() == MInstruction::H_ICMP) {
            auto icmp = static_cast<MHIicmp *>(cond);
            auto opd1 = icmp->getReg(0);
            auto opd2 = icmp->getReg(1);
            if (cond->getUses().size() == 2) { // only used by itself and br
              // remove condi and replace jmp
              MInstruction *mi;
              switch (icmp->getOpTag()) {
              case OpTag::EQ: {
                mi = new MIbeq(opd1, opd2, tb);
                break;
              }
              case OpTag::NE: {
                mi = new MIbne(opd1, opd2, tb);
                break;
              }
              case OpTag::SLE: {
                mi = new MIbge(opd2, opd1, tb);
                break;
              }
              case OpTag::SLT: {
                mi = new MIblt(opd2, opd1, tb);
                break;
              }
              case OpTag::SGE: {
                mi = new MIbge(opd1, opd2, tb);
                break;
              }
              case OpTag::SGT: {
                mi = new MIblt(opd2, opd1, tb);
                break;
              }
              default:
                assert(0);
              }
              auto j = new MIj(fb);
              condi->replaceWith({});
              mbb->clearJmps();
              mbb->pushJmp(mi);
              mbb->pushJmp(j);
              continue;
            }
          }
        }
      }
      // replace jmp with beq and j
      mbb->clearJmps();
      mbb->pushJmp(new MIbne(cond, Register::reg_zero, tb));
      mbb->pushJmp(new MIj(fb));
    }
  }

  // 2.2 lowering
  for (auto &mbb : mfunc->getBasicBlocks()) {
    for (auto &ins : mbb->getInstructions()) {
      if (ins->getInsTag() == MInstruction::H_ICMP) {
        auto icmp = static_cast<MHIicmp *>(&*ins);
        auto opd1 = icmp->getReg(0);
        auto opd2 = icmp->getReg(1);
        Register *res;
        vector<MInstruction *> instrs;
        switch (icmp->getOpTag()) {
        case OpTag::EQ: {
          auto sub = new MIsubw(opd1, opd2);
          auto eq = new MIsltiu(sub, 1, icmp->getName());
          instrs.push_back(sub);
          instrs.push_back(eq);
          res = eq;
          break;
        }
        case OpTag::NE: {
          auto sub = new MIsubw(opd1, opd2);
          auto ne = new MIsltu(Register::reg_zero, sub, icmp->getName());
          instrs.push_back(sub);
          instrs.push_back(ne);
          res = ne;
          break;
        }
        case OpTag::SLE: {
          auto slt = new MIslt(opd2, opd1);
          auto sle = new MIxori(slt, 1, icmp->getName());
          instrs.push_back(slt);
          instrs.push_back(sle);
          res = sle;
          break;
        }
        case OpTag::SLT: {
          auto slt = new MIslt(opd1, opd2);
          instrs.push_back(slt);
          res = slt;
          break;
        }
        case OpTag::SGE: {
          auto sgt = new MIslt(opd2, opd1);
          auto sle = new MIxori(sgt, 1, icmp->getName());
          instrs.push_back(sgt);
          instrs.push_back(sle);
          res = sle;
          break;
        }
        case OpTag::SGT: {
          auto sgt = new MIslt(opd2, opd1);
          instrs.push_back(sgt);
          res = sgt;
          break;
        }
        default:
          assert(0);
        }
        icmp->replaceRegisterWith(res);
        icmp->replaceWith(instrs);
      }
    }
  }
}

Register *get_vreg(MModule *m, MBasicBlock *mbb, Value *v,
                   map<Instruction *, Register *> *instr_map, MFunction *func) {

  if (v->getValueTag() == VT_ARG) {
    auto tp = v->getType();
    auto func_tp = static_cast<FuncType *>(func->getType());
    int cnt = 0;
    bool found = false;
    for (int i = 0; i < func_tp->getArgSize(); i++) {
      if (func_tp->getArgument(i) == v) {
        found = true;
        cnt = i;
        break;
      }
    }
    assert(found);
    return func->getPara(cnt);
  }

  if (v->getValueTag() == VT_INTCONST) {
    ADD_INSTR(i, MIli, static_cast<IntegerConstant *>(v)->getValue());
    return i;
  }

  if (v->getValueTag() == VT_FLOATCONST) {
    auto fc = static_cast<FloatConstant *>(v);
    auto fg = m->addGlobalFloat(fc);
    ADD_INSTR(i, MIflw, fg);
    return i;
  }

  auto ins = static_cast<Instruction *>(v);
  auto it = instr_map->find(ins);
  if (it != instr_map->end()) {
    return it->second;
  } else {
    return new IRRegister(ins);
  }
}

void select_instruction(MModule *res, ANTPIE::Module *ir) {

  // assert ir: every block begin with phi, end with j

  auto instr_map = make_unique<map<Instruction *, Register *>>();
  auto func_map = make_unique<map<Function *, MFunction *>>();
  auto global_map = make_unique<map<GlobalVariable *, MGlobal *>>();

  // Select Global Variables
  auto globalVars = ir->getGlobalVariables();
  for (auto it = globalVars->begin(); it != globalVars->end(); ++it) {
    auto g = *it;
    MGlobal *mg = res->addGlobalVariable(g);
    global_map->insert({g, mg});
  }

  // Select Functions
  auto functions = ir->getFunctions();
  for (auto it = functions->begin(); it != functions->end(); ++it) {
    auto func = *it;
    MFunction *mfunc = res->addFunction(
        static_cast<FuncType *>(func->getType()), func->getName());
    func_map->insert({func, mfunc});
  }

  for (auto it = functions->begin(); it != functions->end();
       ++it) { // Begin Func Loop
    auto func = *it;
    MFunction *mfunc = func_map->at(func);
    auto bb_map = make_unique<map<BasicBlock *, MBasicBlock *>>();
    auto instr_map = make_unique<map<Instruction *, Register *>>();

    // Select BBs
    auto basicBlocks = func->getBasicBlocks();
    for (auto it = basicBlocks->begin(); it != basicBlocks->end(); ++it) {
      auto bb = *it;
      if (bb->isEmpty())
        continue;
      MBasicBlock *mbb =
          mfunc->addBasicBlock(func->getName() + "." + bb->getName());
      bb_map->insert({bb, mbb});
    }
    mfunc->setEntry(bb_map->at(func->getEntry()));

    auto domt = func->getDT();
    auto pr = domt->postOrder();
    auto mdompr = new vector<MBasicBlock *>();
    for (auto bb : *pr) {
      if (bb->isEmpty())
        continue;
      mdompr->push_back(bb_map->at(bb));
    }
    std::reverse(mdompr->begin(), mdompr->end());
    mfunc->domtPreOrder = unique_ptr<vector<MBasicBlock*>>(mdompr);

    // Select every Instruction
    for (auto it = basicBlocks->begin(); it != basicBlocks->end();
         ++it) { // Begin BB Loop
      auto bb = *it;
      if (bb->isEmpty())
        continue;
      MBasicBlock *mbb = bb_map->at(bb);

      auto instrs = bb->getInstructions();
      for (auto it = instrs->begin(); it != instrs->end();
           ++it) { // Begin Instruction Loop
        auto ins = *it;
        // std::cout << "select  ";
        // ins->printIR(std::cout);
        // std::cout << endl;
        switch (ins->getValueTag()) {
          ///////////////////////////////////////////////////////////////////////////////
#define GET_VREG(V) get_vreg(res, mbb, V, &*instr_map, mfunc)

#define BINARY_OP_WITH_IMM_CASE(OP, INSTR, INSTR_I, CONST_TP)                  \
  case OP: {                                                                   \
    MInstruction *BIN_INS;                                                     \
    auto o1c = is_constant(opd1);                                              \
    auto o2c = is_constant(opd2);                                              \
    if (o1c && o2c) {                                                          \
      auto imm = static_cast<CONST_TP *>(opd2)->getValue();                    \
      ADD_INSTR(_bin, INSTR_I, GET_VREG(opd1), imm, ins->getName());           \
      BIN_INS = _bin;                                                          \
    } else if (o1c) {                                                          \
      auto imm = static_cast<CONST_TP *>(opd1)->getValue();                    \
      ADD_INSTR(_bin, INSTR_I, GET_VREG(opd2), imm, ins->getName());           \
      BIN_INS = _bin;                                                          \
    } else if (o2c) {                                                          \
      auto imm = static_cast<CONST_TP *>(opd2)->getValue();                    \
      ADD_INSTR(_bin, INSTR_I, GET_VREG(opd1), imm, ins->getName());           \
      BIN_INS = _bin;                                                          \
    } else {                                                                   \
      ADD_INSTR(_bin, INSTR, GET_VREG(opd1), GET_VREG(opd2), ins->getName());  \
      BIN_INS = _bin;                                                          \
    }                                                                          \
    instr_map->insert({ins, BIN_INS});                                         \
    break;                                                                     \
  }

#define BINARY_OP_CASE(OP, INSTR)                                              \
  case OP: {                                                                   \
    ADD_INSTR(i, INSTR, GET_VREG(opd1), GET_VREG(opd2), ins->getName());       \
    instr_map->insert({ins, i});                                               \
    break;                                                                     \
  }

#define BINARY_OP_REVERSE(OP, INSTR)                                           \
  case OP: {                                                                   \
    ADD_INSTR(i, INSTR, GET_VREG(opd2), GET_VREG(opd1), ins->getName());       \
    instr_map->insert({ins, i});                                               \
    break;                                                                     \
  }
          //////////////////////////////
          //////// Instr Cases /////////
        case VT_JUMP: {
          JumpInst *jmp = static_cast<JumpInst *>(ins);
          auto bb = static_cast<BasicBlock *>(jmp->getRValue(0));
          auto targetbb = bb_map->at(bb);
          auto mjmp = new MIj(targetbb);
          mbb->pushJmp(mjmp);
          break;
        }
        case VT_BR: {
          BranchInst *br = static_cast<BranchInst *>(ins);
          auto cond = GET_VREG(br->getRValue(0));
          auto if_true =
              bb_map->at(static_cast<BasicBlock *>(br->getRValue(1)));
          auto if_false =
              bb_map->at(static_cast<BasicBlock *>(br->getRValue(2)));
          auto mbr = new MHIbr(cond, if_true, if_false);
          mbb->pushJmp(mbr);
          break;
        }
        case VT_RET: {
          ReturnInst *ret = static_cast<ReturnInst *>(ins);
          auto ret_val = ret->getRValue(0);
          MHIret *mret;
          if (ret_val->getValueTag() == VT_FLOATCONST) {
            auto i = static_cast<IntegerConstant *>(ret_val)->getValue();
            mret = new MHIret(i);
          } else if (ret_val->getValueTag() == VT_INTCONST) {
            auto f = static_cast<FloatConstant *>(ret_val)->getValue();
            mret = new MHIret(f);
          } else {
            mret = new MHIret(GET_VREG(ret_val));
          }
          mbb->pushJmp(mret);
          break;
        }
        case VT_CALL: {
          // return type can not be pointer in sysy
          CallInst *call = static_cast<CallInst *>(ins);
          Register::RegTag retTag;
          switch (call->getType()->getTypeTag()) {
          case TT_INT1:
          case TT_INT32:
          case TT_POINTER: {
            retTag = Register::RegTag::I_REGISTER;
            break;
          }
          case TT_FLOAT: {
            retTag = Register::RegTag::I_REGISTER;
            break;
          }
          default:
            assert(0);
          }
          auto callee = func_map->at(call->getFunction());
          ADD_INSTR(mcall, MHIcall, callee, call->getName(), retTag);
          for (int i = 0; i < call->getRValueSize(); i++) {
            auto arg = call->getRValue(i);
            if (arg->getValueTag() == VT_FLOATCONST) {
              auto f = static_cast<FloatConstant *>(arg)->getValue();
              mcall->pushArg(f);
            } else if (arg->getValueTag() == VT_INTCONST) {
              auto i = static_cast<IntegerConstant *>(arg)->getValue();
              mcall->pushArg(i);
            } else {
              mcall->pushArg(GET_VREG(arg));
            }
          }
          instr_map->insert({ins, mcall});
          break;
        }
        case VT_FPTOSI: {
          FptosiInst *i = static_cast<FptosiInst *>(ins);
          auto f = GET_VREG(i->getRValue(0));
          ADD_INSTR(fcvtw_s, MIfcvtw_s, f, ins->getName());
          instr_map->insert({ins, fcvtw_s});
          break;
        }
        case VT_SITOFP: {
          SitofpInst *i = static_cast<SitofpInst *>(ins);
          auto s = GET_VREG(i->getRValue(0));
          ADD_INSTR(fcvts_w, MIfcvts_w, s, ins->getName());
          instr_map->insert({ins, fcvts_w});
          break;
        }
        case VT_ALLOCA: {
          AllocaInst *alloca = static_cast<AllocaInst *>(ins);
          auto tp = alloca->getType();
          if (tp->getTypeTag() == TT_POINTER) {
            auto pt = static_cast<PointerType *>(tp);
            tp = pt->getElemType();
          }
          auto size = cal_size(tp);
          ADD_INSTR(malloca, MHIalloca, size, ins->getName());
          instr_map->insert({ins, malloca});
          break;
        }
        case VT_LOAD: {
          LoadInst *load = static_cast<LoadInst *>(ins);
          auto tp = load->getType();
          auto addr = load->getRValue(0);
          if (addr->getValueTag() == VT_GLOBALVAR) {
            auto g = global_map->at(static_cast<GlobalVariable *>(addr));
            if (tp == Type::getFloatType()) {
              ADD_INSTR(mload, MIflw, g, ins->getName());
              instr_map->insert({ins, mload});
            } else if (tp == Type::getInt32Type() ||
                       tp == Type::getInt1Type()) {
              ADD_INSTR(mload, MIlw, g, ins->getName());
              instr_map->insert({ins, mload});
            } else {
              assert(0);
              ADD_INSTR(mload, MIld, g, ins->getName());
              instr_map->insert({ins, mload});
            }
          } else {
            auto a = GET_VREG(addr);
            if (tp == Type::getFloatType()) {
              ADD_INSTR(mload, MIflw, a, 0, ins->getName());
              instr_map->insert({ins, mload});
            } else if (tp == Type::getInt32Type() ||
                       tp == Type::getInt1Type()) {
              ADD_INSTR(mload, MIlw, a, 0, ins->getName());
              instr_map->insert({ins, mload});
            } else {
              assert(0);
              ADD_INSTR(mload, MIld, a, 0, ins->getName());
              instr_map->insert({ins, mload});
            }
          }
          break;
        }
        case VT_STORE: {
          StoreInst *store = static_cast<StoreInst *>(ins);
          auto value = store->getRValue(0);
          auto addr = store->getRValue(1);
          auto v = GET_VREG(value);
          if (addr->getValueTag() == VT_GLOBALVAR) {
            auto g = global_map->at(static_cast<GlobalVariable *>(addr));
            if (value->getType() == Type::getFloatType()) {
              ADD_INSTR(_, MIfsw, v, g);
            } else if (value->getType() == Type::getInt32Type() ||
                       value->getType() == Type::getInt1Type()) {
              ADD_INSTR(_, MIsw, v, g);
            } else {
              ADD_INSTR(_, MIsd, v, g);
            }
          } else {
            auto a = GET_VREG(addr);
            if (value->getType() == Type::getFloatType()) {
              ADD_INSTR(_, MIfsw, v, 0, a);
            } else if (value->getType() == Type::getInt32Type() ||
                       value->getType() == Type::getInt1Type()) {
              ADD_INSTR(_, MIsw, v, 0, a);
            } else {
              ADD_INSTR(_, MIsd, v, 0, a);
            }
          }
          break;
        }
        case VT_GEP: {
          GetElemPtrInst *gep = static_cast<GetElemPtrInst *>(ins);

          Register *base = GET_VREG(gep->getRValue(0));

          const Type *current_type = gep->getPtrType();
          auto ptrtp = static_cast<const PointerType *>(current_type);
          current_type = ptrtp->getElemType();
          Register *dest = base;
          for (unsigned i = 1; i < gep->getRValueSize(); i++) {
            auto sz =  cal_size(current_type);

           if (i != gep->getRValueSize() - 1) {
              current_type =
                  static_cast<const ArrayType *>(current_type)->getElemType();
            }
        
            auto idx = gep->getRValue(i);
            if (idx->getValueTag() == VT_INTCONST) {
              auto v = static_cast<IntegerConstant *>(idx)->getValue();
              if (v == 0) {
                continue;
              } // what if v == 1? ==> optimize it in peephole optimization...
            } 

            Register *index = GET_VREG(gep->getRValue(i));
            ADD_INSTR(elesz, MIli, sz);
            ADD_INSTR(offset, MImul, index, elesz);
            ADD_INSTR(addr, MIadd, base, offset);
            dest = addr;
          }
          dest->setName(ins->getName());
          instr_map->insert({ins, dest});
          break;
        }
        case VT_PHI: {
          PhiInst *phi = static_cast<PhiInst *>(ins);
          Register::RegTag retTag;
          bool is_pointer = false;
          switch (phi->getRValue(0)->getType()->getTypeTag()) {
          case TT_INT1:
          case TT_INT32: {
            retTag = Register::RegTag::V_IREGISTER;
            break;
          }
          case TT_POINTER: {
            is_pointer = true;
            retTag = Register::RegTag::V_IREGISTER;
            break;
          }
          case TT_FLOAT: {
            retTag = Register::RegTag::V_FREGISTER;
            break;
          }
          default:
            assert(0);
          }
          auto mphi = new MHIphi(phi->getName(), retTag, is_pointer);
          for (int i = 0; i < phi->getRValueSize(); i += 2) {
            Value *opd = phi->getRValue(i);
            BasicBlock *pred_bb =
                static_cast<BasicBlock *>(phi->getRValue(i + 1));
            auto *mbb = bb_map->at(pred_bb);
            if (opd->getValueTag() == VT_FLOATCONST) {
              auto f = static_cast<FloatConstant *>(opd)->getValue();
              mphi->pushIncoming(f, mbb);
            } else if (opd->getValueTag() == VT_INTCONST) {
              auto i = static_cast<IntegerConstant *>(opd)->getValue();
              mphi->pushIncoming(i, mbb);
            } else {
              mphi->pushIncoming(GET_VREG(opd), mbb);
            }
          }
          mbb->pushPhi(mphi);
          instr_map->insert({ins, mphi});
          break;
        }
        case VT_ZEXT: {
          ZextInst *zt = static_cast<ZextInst *>(ins);
          Register *oprand = GET_VREG(zt->getRValue(0));
          instr_map->insert({ins, oprand});
          break;
        }
        case VT_ICMP: {
          IcmpInst *icmp = static_cast<IcmpInst *>(ins);
          // slti can be placed after peephole opt
          auto opd1 = GET_VREG(icmp->getRValue(0));
          auto opd2 = GET_VREG(icmp->getRValue(1));
          ADD_INSTR(micmp, MHIicmp, icmp->getOpTag(), opd1, opd2,
                    icmp->getName());
          instr_map->insert({ins, micmp});
          break;
        }
        case VT_FCMP: {
          FcmpInst *fcmp = static_cast<FcmpInst *>(ins);
          auto opd1 = fcmp->getRValue(0);
          auto opd2 = fcmp->getRValue(1);
          switch (fcmp->getOpTag()) {
            BINARY_OP_CASE(OEQ, MIfeq_s)
          case ONE: {
            ADD_INSTR(feq, MIfeq_s, GET_VREG(opd1), GET_VREG(opd2),
                      ins->getName());
            ADD_INSTR(ne, MIxori, feq, 1, ins->getName());
            instr_map->insert({ins, ne});
            break;
          }
            BINARY_OP_CASE(OLT, MIflt_s)
            BINARY_OP_CASE(OLE, MIfle_s)
            BINARY_OP_REVERSE(OGT, MIflt_s)
            BINARY_OP_REVERSE(OGE, MIfle_s)
          default:
            assert(0);
          }
          break;
        }
        case VT_BOP: {
          BinaryOpInst *bins = static_cast<BinaryOpInst *>(ins);
          auto opd1 = bins->getRValue(0);
          auto opd2 = bins->getRValue(1);
          switch (bins->getOpTag()) {
            BINARY_OP_WITH_IMM_CASE(ADD, MIaddw, MIaddiw, IntegerConstant)
            BINARY_OP_WITH_IMM_CASE(AND, MIand, MIandi, IntegerConstant)
            BINARY_OP_WITH_IMM_CASE(OR, MIor, MIori, IntegerConstant)
            BINARY_OP_WITH_IMM_CASE(XOR, MIxor, MIxori, IntegerConstant)
            BINARY_OP_CASE(SUB, MIsubw)
            BINARY_OP_CASE(MUL, MImulw)
            BINARY_OP_CASE(SDIV, MIdivw)
            BINARY_OP_CASE(SREM, MIremw)
            BINARY_OP_CASE(FADD, MIfadd_s)
            BINARY_OP_CASE(FSUB, MIfsub_s)
            BINARY_OP_CASE(FMUL, MIfmul_s)
            BINARY_OP_CASE(FDIV, MIfdiv_s)
          case FREM: {
            assert(0);
          }
          default:
            assert(0);
          }
          break;
        }
          ///////////////////////////////////////////////////////////////////////////////
        default:
          assert(0);
        }
      } // End Instruction Loop
    } // End BB Loop

    // std::cout << "Reslove IRRegisters to VRegisters" << endl;
    // 1. Reslove IRRegisters to VRegisters
    for (auto &mbb : mfunc->getBasicBlocks()) {
      for (auto &mins : mbb->getInstructions()) {
        mins->replaceIRRegister(*instr_map);
      }
    }

    // std::cout << "lower H_ICMP and BR" << endl;
    // 2. lower H_ICMP and BR
    lowerHIicmp(mfunc);

  } // End Func Loop
}