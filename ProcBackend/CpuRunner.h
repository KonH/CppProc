#pragma once

template<int BS, int IMS, int RMS>
class CpuRunner {
public:
	bool Tick(const RegisterSet<BS, IMS>& regs, MemoryState<IMS>& cpu, MemoryState<RMS>& ram) {
		if (cpu.get(regs.Terminated).test(0)) {
			return false; // Terminated
		}
		auto ip = cpu.get(regs.IP);
		Reference<BS * 3> command(ip.to_ulong()); // Command code + 2 args
		auto command_body = ram.get(command);
		perform_command(regs, cpu, ram, command_body);
		// todo: check invalid references
		// todo: increase counter
		return true; // Continue execution
	}

private:
	void perform_command(const RegisterSet<BS, IMS>& regs, MemoryState<IMS>& cpu, MemoryState<RMS>& ram, const bitset<BS * 3>& command) {
		// todo: dispatch
		auto code = BitUtils::get_bits<BS, BS * 3>(command, 0);
		auto arg1 = BitUtils::get_bits<BS, BS * 3>(command, BS);
		auto arg2 = BitUtils::get_bits<BS, BS * 3>(command, BS * 2);

		switch (code.to_ulong()) {
		case 0b0001:
			cpu.set(regs.Terminated, bitset<1>(0b1)); // temp
			// todo: increase pointer
			break;
		default:
			break;
		}
	}
};