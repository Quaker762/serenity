/*
 * Copyright (c) 2018-2021, James Mintram <me@jamesrm.com>
 * Copyright (c) 2018-2021, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Function.h>
#include <Kernel/Arch/DeferredCallEntry.h>
#include <Kernel/VirtualAddress.h>
#include <Kernel/Arch/ProcessorSpecificDataID.h>

namespace Kernel {

struct [[gnu::aligned(16)]] FPUState
{
    u8 buffer[512];
};

class Processor;
struct ProcessorMessage;
struct ProcessorMessageEntry;
class x86Processor;
class Processor
{
    friend class ProcessorInfo;
    friend class x86Processor;

    AK_MAKE_NONCOPYABLE(Processor);
    AK_MAKE_NONMOVABLE(Processor);

public:
    Processor() = default;

    void early_initialize(u32 cpu);
    void initialize(u32 cpu);

    template<typename T>
    T* get_specific();

    void set_specific(ProcessorSpecificDataID specific_id, void* ptr);

    ALWAYS_INLINE static void pause();
    ALWAYS_INLINE static void wait_check();
    ALWAYS_INLINE u8 physical_address_bit_width() const;
    ALWAYS_INLINE u8 virtual_address_bit_width() const;
    ALWAYS_INLINE static bool is_initialized();
    ALWAYS_INLINE static void flush_tlb_local(VirtualAddress&, size_t&);
    ALWAYS_INLINE static void flush_tlb(Memory::PageDirectory const*, VirtualAddress const&, size_t);
    ALWAYS_INLINE static u32 current_id();
    ALWAYS_INLINE static Thread* current_thread();
    ALWAYS_INLINE static FlatPtr current_in_irq();
    ALWAYS_INLINE static u64 read_cpu_counter();
    ALWAYS_INLINE static Thread* idle_thread();
    ALWAYS_INLINE static Processor& current();
    ALWAYS_INLINE static void leave_critical();
    ALWAYS_INLINE static u32 clear_critical();
    ALWAYS_INLINE static void restore_critical(u32 prev_critical);
    ALWAYS_INLINE static u32 in_critical();
    ALWAYS_INLINE static FPUState const& clean_fpu_state();
    static bool is_smp_enabled();

    [[noreturn]] static void halt();

    static void smp_enable();
    bool smp_process_pending_messages();
    static void smp_unicast(u32 cpu, Function<void()>, bool async);
    static void smp_broadcast_flush_tlb(Memory::PageDirectory const*, VirtualAddress, size_t);
    static u32 smp_wake_n_idle_processors(u32 wake_count);

    static void deferred_call_queue(Function<void()> callback);

    ALWAYS_INLINE bool has_nx() const;
    ALWAYS_INLINE bool has_pat() const;

    //ALWAYS_INLINE bool has_feature(CPUFeature::Type const& feature) const;

    void check_invoke_scheduler();
    void invoke_scheduler_async();

    void enter_trap(TrapFrame& trap, bool raise_irq);
    void exit_trap(TrapFrame& trap);

    [[noreturn]] void initialize_context_switching(Thread& initial_thread);
    NEVER_INLINE void switch_context(Thread*& from_thread, Thread*& to_thread);
    [[noreturn]] static void assume_context(Thread& thread, FlatPtr flags);
    FlatPtr init_context(Thread& thread, bool leave_crit);
    static ErrorOr<Vector<FlatPtr, 32>> capture_stack_trace(Thread& thread, size_t max_frames = 0);

    static StringView platform_string();

    alignas(16) u8 m_data[4 * KiB]; // 4KiB data buffer for Architecture specific data

private:
    Processor* m_self;

    u32 m_cpu;
    FlatPtr m_in_irq;
    volatile u32 m_in_critical;
    static Atomic<u32> s_idle_cpu_mask;
    static Atomic<u32> g_total_processors;
    u8 m_physical_address_bit_width;
    u8 m_virtual_address_bit_width;
    Thread* m_current_thread;
    Thread* m_idle_thread;

    Atomic<ProcessorMessageEntry*> m_message_queue;

    bool m_invoke_scheduler_async;
    bool m_scheduler_initialized;
    bool m_in_scheduler;
    Atomic<bool> m_halt_requested;
};

namespace Memory {
class PageDirectory;
}

struct ProcessorMessageEntry;
struct ProcessorMessage {
    using CallbackFunction = Function<void()>;

    enum Type {
        FlushTlb,
        Callback,
    };
    Type type;
    Atomic<u32> refs;
    union {
        ProcessorMessage* next; // only valid while in the pool
        alignas(CallbackFunction) u8 callback_storage[sizeof(CallbackFunction)];
        struct {
            Memory::PageDirectory const* page_directory;
            u8* ptr;
            size_t page_count;
        } flush_tlb;
    };

    bool volatile async;

    ProcessorMessageEntry* per_proc_entries;

    CallbackFunction& callback_value()
    {
        return *bit_cast<CallbackFunction*>(&callback_storage);
    }

    void invoke_callback()
    {
        VERIFY(type == Type::Callback);
        callback_value()();
    }
};

struct ProcessorMessageEntry {
    ProcessorMessageEntry* next;
    ProcessorMessage* msg;
};

template<typename T>
class ProcessorSpecific {
public:
    static void initialize()
    {
        Processor::current().set_specific(T::processor_specific_data_id(), new T);
    }
    static T& get()
    {
        return *Processor::current().get_specific<T>();
    }
};
}
