#include "MachineInstruction.hh"

/////////////////////////////////////////////////
//
//                MachineBasicBlock
//
/////////////////////////////////////////////////

void MachineBasicBlock::printASM(ostream &stream) const {
  // do some thing
  for (const auto &instr : *instructions) {
    stream << "  ";
    instr->printASM(stream);
    stream << endl;
  }
}

/////////////////////////////////////////////////
//
//                MachineInstruction
//
/////////////////////////////////////////////////

void MachineInstruction::pushReg(Register *r) { oprands->push_back(r); }

void MachineInstruction::pushJTarget(MachineBasicBlock *b) {
  j_targets->push_back(b);
}

void MachineInstruction::setImm(Immediate i) {
  imm = make_unique<Immediate>(i);
}

void MachineInstruction::setFunction(MachineFunction *fun_) { fun = fun_; }

MachineInstruction::MachineInstructionTag MachineInstruction::getTag() const {
  return tag;
}

Register *MachineInstruction::getReg(int idx) const { return oprands->at(idx); }

MachineBasicBlock *MachineInstruction::getJTarget(int idx) const {
  return j_targets->at(idx);
}

Immediate *MachineInstruction::getImm() const { return &*imm; }

void MachineInstruction::printASM(std::ostream &stream) const {
  switch (tag) {
    case MachineInstruction::PHI: {
      stream << "phi";
      break;
    }
    case MachineInstruction::ADDIW: {
      stream << "addiw";
      break;
    }
    case MachineInstruction::ADDW: {
      stream << "addw";
      break;
    }
    case MachineInstruction::SUBW: {
      stream << "subw";
      break;
    }
    case MachineInstruction::AND: {
      stream << "and";
      break;
    }
    case MachineInstruction::OR: {
      stream << "or";
      break;
    }
    case MachineInstruction::XOR: {
      stream << "xor";
      break;
    }
    case MachineInstruction::ANDI: {
      stream << "andi";
      break;
    }
    case MachineInstruction::ORI: {
      stream << "ori";
      break;
    }
    case MachineInstruction::XORI: {
      stream << "xori";
      break;
    }
    case MachineInstruction::SLLW: {
      stream << "sllw";
      break;
    }
    case MachineInstruction::SRAW: {
      stream << "sraw";
      break;
    }
    case MachineInstruction::SRLW: {
      stream << "srlw";
      break;
    }
    case MachineInstruction::SLLIW: {
      stream << "slliw";
      break;
    }
    case MachineInstruction::SRAIW: {
      stream << "sraiw";
      break;
    }
    case MachineInstruction::SRLIW: {
      stream << "srliw";
      break;
    }
    case MachineInstruction::LUI: {
      stream << "lui";
      break;
    }
    case MachineInstruction::AUIPC: {
      stream << "auipc";
      break;
    }
    case MachineInstruction::SLT: {
      stream << "slt";
      break;
    }
    case MachineInstruction::SLTI: {
      stream << "slti";
      break;
    }
    case MachineInstruction::SLTU: {
      stream << "sltu";
      break;
    }
    case MachineInstruction::SLTIU: {
      stream << "sltiu";
      break;
    }
    case MachineInstruction::LW: {
      stream << "lw";
      break;
    }
    case MachineInstruction::SW: {
      stream << "sw";
      break;
    }
    case MachineInstruction::BEQ: {
      stream << "beq";
      break;
    }
    case MachineInstruction::BNE: {
      stream << "bne";
      break;
    }
    case MachineInstruction::BGE: {
      stream << "bge";
      break;
    }
    case MachineInstruction::BLT: {
      stream << "blt";
      break;
    }
    case MachineInstruction::BGEU: {
      stream << "bgeu";
      break;
    }
    case MachineInstruction::BLTU: {
      stream << "bltu";
      break;
    }
    case MachineInstruction::JAL: {
      stream << "jal";
      break;
    }
    case MachineInstruction::JALR: {
      stream << "jalr";
      break;
    }
    case MachineInstruction::CALL: {
      stream << "call";
      break;
    }
    case MachineInstruction::J: {
      stream << "j";
      break;
    }
    case MachineInstruction::RET: {
      stream << "ret";
      break;
    }
    case MachineInstruction::MULW: {
      stream << "mulw";
      break;
    }
    case MachineInstruction::MULHU: {
      stream << "mulhu";
      break;
    }
    case MachineInstruction::MULHSU: {
      stream << "mulhsu";
      break;
    }
    case MachineInstruction::DIVW: {
      stream << "divw";
      break;
    }
    case MachineInstruction::REMW: {
      stream << "remw";
      break;
    }
    case MachineInstruction::DIVUW: {
      stream << "divuw";
      break;
    }
    case MachineInstruction::REMUW: {
      stream << "remuw";
      break;
    }
    case MachineInstruction::FADD_S: {
      stream << "fadd.s";
      break;
    }
    case MachineInstruction::FSUB_S: {
      stream << "fsub.s";
      break;
    }
    case MachineInstruction::FMUL_S: {
      stream << "fmul.s";
      break;
    }
    case MachineInstruction::FDIV_S: {
      stream << "fdiv.s";
      break;
    }
    case MachineInstruction::FSQRT_S: {
      stream << "fsqrt.s";
      break;
    }
    case MachineInstruction::FMIN_S: {
      stream << "fmin.s";
      break;
    }
    case MachineInstruction::FMAX_S: {
      stream << "fmax.s";
      break;
    }
    case MachineInstruction::FNMADD_S: {
      stream << "fnmadd.s";
      break;
    }
    case MachineInstruction::FNMSUB_S: {
      stream << "fnmsub.s";
      break;
    }
    case MachineInstruction::FMV_S_X: {
      stream << "fmv.s.x";
      break;
    }
    case MachineInstruction::FMV_X_S: {
      stream << "fmv.x.s";
      break;
    }
    case MachineInstruction::FLW: {
      stream << "flw";
      break;
    }
    case MachineInstruction::FSW: {
      stream << "fsw";
      break;
    }
    case MachineInstruction::FCVTS_W: {
      stream << "fcvt.s.w";
      break;
    }
    case MachineInstruction::FCVTS_WU: {
      stream << "fcvt.s.wu";
      break;
    }
    case MachineInstruction::FCVTW_S: {
      stream << "fcvt.w.s";
      break;
    }
    case MachineInstruction::FCVTWU_S: {
      stream << "fcvt.wu.s";
      break;
    }
    case MachineInstruction::FEQ_S: {
      stream << "feq.s";
      break;
    }
    case MachineInstruction::FLT_S: {
      stream << "flt.s";
      break;
    }
    case MachineInstruction::FLE_S: {
      stream << "fle.s";
      break;
    }
  }
}

void MachineInstruction::setGlobal(MachineGlobal *global_) { global = global_; }

MachineGlobal *MachineInstruction::getGlobal() const { return global; }

MachineFunction *MachineInstruction::getFunction() const { return fun; }

void MachineInstruction::setTarget(Register *reg) { target = reg; }

Register *MachineInstruction::getTarget(Register *reg) {
  if (target == nullptr) {
    return this;
  } else {
    return target;
  }
}
