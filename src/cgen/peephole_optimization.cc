#include "allocate_register.hh"
#include "cgen.hh"
#include <unordered_set>
using std::unordered_set;

bool is_store(MInstruction *ins) {
  return ins->getInsTag() == MInstruction::SW ||
         ins->getInsTag() == MInstruction::SD ||
         ins->getInsTag() == MInstruction::FSW;
}
int get_store_imm(MInstruction *store) {
  switch (store->getInsTag()) {
  case MInstruction::SW: {
    auto sw = static_cast<MIsw *>(store);
    return sw->imm;
  }
  case MInstruction::SD: {
    auto sd = static_cast<MIsd *>(store);
    return sd->imm;
  }
  case MInstruction::FSW: {
    auto fsw = static_cast<MIfsw *>(store);
    return fsw->imm;
  }
  default:
    assert(0);
  }
}
bool is_load(MInstruction *ins) {
  return ins->getInsTag() == MInstruction::LW ||
         ins->getInsTag() == MInstruction::LD ||
         ins->getInsTag() == MInstruction::FLW;
}
int get_load_imm(MInstruction *load) {
  switch (load->getInsTag()) {
  case MInstruction::LW: {
    auto lw = static_cast<MIlw *>(load);
    return lw->imm;
  }
  case MInstruction::LD: {
    auto ld = static_cast<MIld *>(load);
    return ld->imm;
  }
  case MInstruction::FLW: {
    auto lfw = static_cast<MIflw *>(load);
    return lfw->imm;
  }
  default:
    assert(0);
  }
}

void scan_store_loads(MBasicBlock *bb, vector<vector<MInstruction *>> &ldsts) {
  int stk_off = 0;

  unordered_map<Register *, bool> hold_val; // if reg also hold the value?
  unordered_map<Register *, int> reg_addr;
  unordered_map<int, Register *> addr_reg;
  // invarient: addr always holds the value
  map<Register *, vector<MInstruction *>>
      tldsts; // a list of load/stores which its value never changes

  for (auto ins : bb->getInstructions()) {
    if (ins->getInsTag() == MInstruction::ADDI &&
        ins->getTarget() == Register::reg_sp) {
      int imm = static_cast<MIaddi *>(ins)->imm;
      stk_off += imm;
      continue;
    }
    if (stk_off == 0) { // not in call sequence

      if (is_store(ins) && ins->getReg(1) == Register::reg_sp) {
        int off = get_store_imm(ins);
        auto reg = ins->getReg(0);
        if (reg == Register::reg_ra || reg == Register::reg_s0) {
          continue;
        }
        int addr = stk_off + off;
        if (addr_reg.count(addr) > 0) {
          if (addr_reg[addr] == reg) {
            if (hold_val[reg]) { // find a redudent store
              // std::cout << endl << " Redudent store: " << *ins << endl;
              tldsts[reg].push_back(ins);
              // std::cout <<reg->getName() << " :"  << tldsts[reg].size() <<
              // endl;
              continue;
            }
          }

          // std::cout << endl << " Overwrite store: " << *ins << endl;
          // value was overwrite
          if (reg_addr.count(reg) > 0) {
            ldsts.push_back(tldsts[reg]);
            tldsts[reg].clear();
            addr_reg.erase(reg_addr[reg]);
            reg_addr.erase(reg);
          }
          ldsts.push_back(tldsts[addr_reg[addr]]);
          tldsts[addr_reg[addr]].clear();
          auto erase = addr_reg[addr];
          addr_reg.erase(addr);
          reg_addr.erase(erase);
          // std::cout << reg_addr.count(reg) << endl;

          tldsts[reg].push_back(ins);
          hold_val[reg] = false; // only load will be condsidered hold value
          reg_addr[reg] = addr;
          addr_reg[addr] = reg;
          // std::cout <<reg->getName() << " :"  << tldsts[reg].size() << endl;
        } else if (reg_addr.count(reg) > 0) {
          // reg store cllapse... ignore new store..
          // std::cout << endl << reg->getName() << endl;
          // std::cout << reg_addr[reg] << endl;
          // std::cout << addr << endl;
          // std::cout << *ins << endl;
          // std::cout <<reg->getName() << " :"  << tldsts[reg].size() << endl;

          assert(tldsts[reg].size() != 0);
        } else {
          // std::cout << endl << " New store: " << *ins << endl;
          // no store here  add new
          if (tldsts.find(reg) == tldsts.end()) {
            tldsts.insert({reg, {}});
          }
          assert(tldsts[reg].size() == 0);
          hold_val[reg] = false;
          tldsts[reg].push_back(ins);
          reg_addr[reg] = addr;
          addr_reg[addr] = reg;
          // std::cout <<reg->getName() << " :"  << tldsts[reg].size() << endl;
        }
        continue;
      } else if (is_load(ins) && ins->getReg(0) == Register::reg_sp) {
        auto reg = ins->getTarget();
        auto off = get_load_imm(ins);
        if (reg == Register::reg_ra || reg == Register::reg_s0) {
          continue;
        }
        auto addr = off + stk_off;
        if (reg_addr.count(reg) != 0 && reg_addr[reg] == addr) {
          // std::cout << endl << " Load what stored before: " << *ins << endl;
          // std::cout << endl <<  reg_addr.count(reg) << endl;
          // std::cout << off << endl;
          // std::cout << stk_off << endl;
          // std::cout << reg_addr[reg] << endl;
          // std::cout << reg->getName() << endl;
          assert(tldsts[reg].size() != 0);
          tldsts[reg].push_back(ins);
          hold_val[reg] = true;
          // std::cout <<reg->getName() <<" :"  << tldsts[reg].size() << endl;
        }
        continue;
      }
    }

    if (ins->getInsTag() == MInstruction::CALL) {
      // TODO: invalidate all caller saved registers...
      if (hold_val.count(Register::reg_a0) > 0) {
        hold_val[Register::reg_a0] = false;
      }
    }

    auto iuses = getUses<Register::I_REGISTER>(ins);
    for (auto reg : iuses) {
      if (reg_addr.count(reg) > 0) { // the reg is in stack
        if (hold_val[reg]) { // the value was used, then the last load can not
                             // be deleted
          // std::cout << endl << " Invalidate: " << *ins << endl;
          ldsts.push_back(tldsts[reg]);
          tldsts[reg].clear();
          addr_reg.erase(reg_addr[reg]);
          reg_addr.erase(reg);
          // std::cout <<reg->getName() << " :"  << tldsts[reg].size() << endl;
        }
      }
    }
    auto fuses = getUses<Register::F_REGISTER>(ins);
    for (auto reg : fuses) {
      if (reg_addr.count(reg) > 0) {
        if (hold_val[reg]) {
          // std::cout << endl << " Invalidate: " << *ins << endl;
          ldsts.push_back(tldsts[reg]);
          tldsts[reg].clear();
          addr_reg.erase(reg_addr[reg]);
          reg_addr.erase(reg);
          // std::cout <<reg->getName() << " :"  << tldsts[reg].size() << endl;
        }
      }
    }
    auto idefs = getDefs<Register::I_REGISTER>(ins);
    for (auto reg : idefs) {
      if (reg_addr.count(reg) > 0) {
        // std::cout << endl << " Kill: " << *ins << endl;
        hold_val[reg] = false;
        // std::cout <<reg->getName() << " :"  << tldsts[reg].size() << endl;
      }
    }
    auto fdefs = getDefs<Register::F_REGISTER>(ins);
    for (auto reg : fdefs) {
      if (reg_addr.count(reg) > 0) {
        // std::cout << endl << " Kill: " << *ins << endl;
        hold_val[reg] = false;
        // std::cout <<reg->getName() << " :"  << tldsts[reg].size() << endl;
      }
    }
  }

  for (auto pair : tldsts) {
    if (pair.second.size() != 0) {
      ldsts.push_back(pair.second);
    }
  }
}

// Be careful!!! The used Register of call Instr in IMPLICIT!!!
void peephole_optimize(MModule *mod) {
  // remove reduant load/stores
  for (auto func : mod->getFunctions()) {
    for (auto bb : func->getBasicBlocks()) {
      vector<vector<MInstruction *>> ldstss;
      scan_store_loads(bb, ldstss);
      // std::cout << "Scan over" << endl;
      for (auto ldsts : ldstss) {
        // std::cout << "Sequence in " << bb->getName() << ": " << endl;
        // for (auto ins : ldsts) {
        //   std::cout << "  " << *ins << endl;
        // }
        int num = ldsts.size();
        int last_ld = -1;
        for (int i = num - 1; i >= 1; i--) {
          if (is_load(ldsts[i])) {
            last_ld = i;
            break;
          }
        }
        if (last_ld != -1) {
          for (int i = 1; i < last_ld; i++) {
            ldsts[i]->replaceWith({});
          }
        }
      }
    }
  }

  for (auto func : mod->getFunctions()) {
    for (auto bb : func->getBasicBlocks()) {
      auto inss = bb->getAllInstructions();
      int i = 0;
      while (i < inss.size()) {
        auto ins = inss[i];
        //////////////////////////////////////
        // mv a0, a0
        // ---------------------------
        // ==> nop
        //////////////////////////////////////
        if (ins->getInsTag() == MInstruction::MV) {
          if (ins->getTarget() == ins->getReg(0)) {
            ins->replaceWith({});
            i++;
            // std::cout << "match " << " mv a0, a0" << endl;
            continue;
          }
        }

        // addi a1, a0, 0 # addi arr.addr.3, arr.addr.2, 0
        // addi a0, a1, 0 # a
        i += 1;
      }
    }
  }
}