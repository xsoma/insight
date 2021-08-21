#pragma once
#include <xlocale>
// Credit's LNK1181


struct Address {
	Address() = default;
	Address(uint8_t* ptr) { m_ptr = ptr; };
	Address(uintptr_t* ptr) { m_ptr = reinterpret_cast<uint8_t*>(ptr); };

	__forceinline operator uint8_t*() { return m_ptr; }

	__forceinline operator void*() { return reinterpret_cast<void*>(m_ptr); }

	__forceinline uint8_t* get() { return m_ptr; }

	template<typename T>
	__forceinline T cast() { return reinterpret_cast<T>(m_ptr); }

	__forceinline uint8_t* offset(int offset) const { return m_ptr + offset; };

	__forceinline Address self_offset(int offset) {
		m_ptr += offset;

		return *this;
	}

	__forceinline uint8_t* jmp(int offset = 0x1) const { return m_ptr + offset + sizeof(uintptr_t) + *reinterpret_cast<int*>(m_ptr + offset); }

	__forceinline Address self_jmp(int offset = 0x1) {
		m_ptr = jmp(offset);

		return *this;
	}

	uint8_t* m_ptr = nullptr;
};