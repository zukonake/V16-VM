#include <stdexcept>
//
#include <instruction.hpp>

Mode::Mode(uint8_t value) :
    type(static_cast<Type>(value & 0b0111)),
    dir(static_cast<Dir>((value & 0b1000) >> 3))
{

}

Mode::Mode(Dir dir, Type type) :
    type(type),
    dir(dir)
{

}

unsigned Mode::getCycleCost() const
{
    unsigned cost = 0;
    switch(dir)
    {
        case D:            break;
        case I:            break; //the change should be equal to MEMORY's cost
        default: throw std::runtime_error("illegal mode dir: " + std::to_string(dir));
    }
    switch(type)
    {
        case M:            break;
        case R:            break;
        case A: cost += 2; break;
        case B: cost += 2; break;
        case P: cost += 2; break;
        case N: cost += 2; break;
        case C:            break;
        default: throw std::runtime_error("illegal mode: " + std::to_string(type));
    }
    return cost;
}

bool Mode::takesWord() const
{
    switch(type)
    {
        case M: return true;
        case R: return true;
        case A: return true;
        case B: return true;
        case P: return true;
        case N: return true;
        case C: return false;
        default: throw std::runtime_error("illegal mode: " + std::to_string(type));
    }
}

Opcode::Opcode(uint8_t value) :
    type(static_cast<Type>(value))
{

}

Opcode::Opcode(Type type) :
    type(type)
{

}

unsigned Opcode::getCycleCost() const
{
    switch(type)
    {
        case NOP: return 1;
        case JMP: return 1;
        case TRM: return 1;
        case CLL: return 2;
        case RET: return 1;
        case MOV: return 1;
        case CPY: return 2;
        case SWP: return 2;
        case IEQ: return 1;
        case INQ: return 1;
        case IGT: return 2;
        case ILT: return 2;
        case IGQ: return 2;
        case ILQ: return 2;
        case NEG: return 1;
        case OR:  return 1;
        case AND: return 1;
        case XOR: return 1;
        case RSH: return 1;
        case LSH: return 1;
        case SPB: return 2;
        case ADD: return 2;
        case SUB: return 2;
        case MUL: return 3;
        case DIV: return 3;
        case MOD: return 4;
        case AIB: return 1;
        case AOB: return 1;
        case AIW: return 2;
        case AOW: return 2;
        default:  throw std::runtime_error("illegal opcode: " + std::to_string(type));
    }
}

unsigned Opcode::getModeSize() const
{
    switch(type)
    {
        case NOP: return 0;
        case JMP: return 1;
        case TRM: return 1;
        case CLL: return 1;
        case RET: return 0;
        case MOV: return 2;
        case CPY: return 2;
        case SWP: return 2;
        case IEQ: return 2;
        case INQ: return 2;
        case IGT: return 2;
        case ILT: return 2;
        case IGQ: return 2;
        case ILQ: return 2;
        case NEG: return 2;
        case OR:  return 2;
        case AND: return 2;
        case XOR: return 2;
        case RSH: return 2;
        case LSH: return 2;
        case SPB: return 1;
        case ADD: return 2;
        case SUB: return 2;
        case MUL: return 2;
        case DIV: return 2;
        case MOD: return 2;
        case AIB: return 2;
        case AOB: return 2;
        case AIW: return 2;
        case AOW: return 2;
        default:  throw std::runtime_error("illegal opcode: " + std::to_string(type));
    }
}

Instruction::Instruction(uint16_t value) :
    opcode((value & 0b1111111100000000) >> 8),
         a((value & 0b0000000011110000) >> 4),
         b (value & 0b0000000000001111)
{

}

Instruction::Instruction(Opcode opcode, Mode a, Mode b, Word x, Word y) :
    opcode(opcode),
    a(a),
    b(b),
    x(x),
    y(y)
{

}

unsigned Instruction::getCycleCost() const
{
    return opcode.getCycleCost() + a.getCycleCost() + b.getCycleCost();
}

unsigned Instruction::getWordSize() const
{
    return 1 + static_cast<unsigned>(a.takesWord()) + static_cast<unsigned>(b.takesWord());
}

