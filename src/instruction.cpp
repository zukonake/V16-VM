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
        case L:            break;
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
        case L: return true;
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
        case NOP:  return 1;
        case JUMP: return 1;
        case TERM: return 1;
        case CALL: return 2;
        case RET:  return 1;
        case PUSH: return 1;
        case POP:  return 1;
        case MOVE: return 1;
        case COPY: return 2;
        case SWAP: return 2;
        case IFEQ: return 1;
        case IFNQ: return 1;
        case IFGT: return 2;
        case IFLT: return 2;
        case IFGQ: return 2;
        case IFLQ: return 2;
        case NEG:  return 1;
        case OR:   return 1;
        case AND:  return 1;
        case XOR:  return 1;
        case RSHF: return 1;
        case LSHF: return 1;
        case SWPB: return 2;
        case ADD:  return 2;
        case SUB:  return 2;
        case MUL:  return 3;
        case DIV:  return 3;
        case MOD:  return 4;
        case ADBI: return 1;
        case ADBO: return 1;
        case ADWI: return 2;
        case ADWO: return 2;
        default:  throw std::runtime_error("illegal opcode: " + std::to_string(type));
    }
}

unsigned Opcode::getModeSize() const
{
    switch(type)
    {
        case NOP:  return 0;
        case JUMP: return 1;
        case TERM: return 1;
        case CALL: return 1;
        case RET:  return 0;
        case PUSH: return 1;
        case POP:  return 1;
        case MOVE: return 2;
        case COPY: return 2;
        case SWAP: return 2;
        case IFEQ: return 2;
        case IFNQ: return 2;
        case IFGT: return 2;
        case IFLT: return 2;
        case IFGQ: return 2;
        case IFLQ: return 2;
        case NEG:  return 2;
        case OR:   return 2;
        case AND:  return 2;
        case XOR:  return 2;
        case RSHF: return 2;
        case LSHF: return 2;
        case SWPB: return 1;
        case ADD:  return 2;
        case SUB:  return 2;
        case MUL:  return 2;
        case DIV:  return 2;
        case MOD:  return 2;
        case ADBI: return 2;
        case ADBO: return 2;
        case ADWI: return 2;
        case ADWO: return 2;
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
    return 1 + static_cast<unsigned>(a.takesWord() && opcode.getModeSize() >= 1) +
               static_cast<unsigned>(b.takesWord() && opcode.getModeSize() >= 2);
}

