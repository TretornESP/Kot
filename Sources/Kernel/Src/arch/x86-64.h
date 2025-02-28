#define MAX_PROCESSORS      256
#define IO_APIC             256
#define MAX_IRQ             256

#define INT_ScheduleAPIC    0x40
#define INT_Schedule        0x41
#define INT_DestroySelf     0x42
#define INT_Stop            0x43

#define PAGE_SIZE (uint64_t)0x1000
#define KERNEL_STACK_SIZE PAGE_SIZE * 0x10 /* 10 MB */

#pragma once


#include <arch/x86-64/asm.h>
#include <arch/x86-64/core.h>
#include <arch/x86-64/io/io.h>
#include <arch/x86-64/smp/smp.h>
#include <arch/x86-64/cpu/cpu.h>
#include <arch/x86-64/gdt/gdt.h>
#include <arch/x86-64/tss/tss.h>
#include <arch/x86-64/vmm/vmm.h>
#include <arch/x86-64/pmm/pmm.h>
#include <arch/x86-64/apic/apic.h>
#include <arch/x86-64/simd/simd.h>
#include <arch/x86-64/io/msr/msr.h>
#include <arch/x86-64/atomic/atomic.h>
#include <arch/x86-64/interrupts/idt.h>
#include <arch/x86-64/io/serial/serial.h>
#include <arch/x86-64/userspace/userspace.h>
#include <arch/x86-64/interrupts/interrupts.h>