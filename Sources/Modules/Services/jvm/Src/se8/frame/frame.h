#pragma once

#include "../types.h"
#include "../jvm.h"

#include "../classes/class.h"
#include "stack.h"
#include "heap.h"

namespace SE8 {

    class JVM;

    class Class;

    struct Frame {
        uint64_t pid;
        uint64_t code_length;
        Stack* stack;
        Locals* locals;
        Class* currentClass;
        Method* currentMethod;
        Reader* reader;
        bool widened;
        void init(Class* cl, Method* method);
        void run(Value* args, uint32_t args_length);
    };

    typedef void (*OpCodeTable)(SE8::Frame*);

    void initOpCodeTable();
    bool isOpCodeTableNotInit();

    namespace Opc {
        void aaload(Frame*);
        void aastore(Frame*);
        void aconst_null(Frame*);
        void aload(Frame*);
        void aload_0(Frame*);
        void aload_1(Frame*);
        void aload_2(Frame*);
        void aload_3(Frame*);
        void anewarray(Frame*);
        void areturn(Frame*);
        void arraylength(Frame*);
        void astore(Frame*);
        void astore_0(Frame*);
        void astore_1(Frame*);
        void astore_2(Frame*);
        void astore_3(Frame*);
        void athrow(Frame*);
        void baload(Frame*);
        void bastore(Frame*);
        void bipush(Frame*);
        void caload(Frame*);
        void castore(Frame*);
        void checkcast(Frame*);
        void d2f(Frame*);
        void d2i(Frame*);
        void d2l(Frame*);
        void dadd(Frame*);
        void daload(Frame*);
        void dastore(Frame*);
        void dcmpg(Frame*);
        void dcmpl(Frame*);
        void dconst_0(Frame*);
        void dconst_1(Frame*);
        void ddiv(Frame*);
        void dload(Frame*);
        void dload_0(Frame*);
        void dload_1(Frame*);
        void dload_2(Frame*);
        void dload_3(Frame*);
        void dmul(Frame*);
        void dneg(Frame*);
        void drem(Frame*);
        void dreturn(Frame*);
        void dstore(Frame*);
        void dstore_0(Frame*);
        void dstore_1(Frame*);
        void dstore_2(Frame*);
        void dstore_3(Frame*);
        void dsub(Frame*);
        void dup(Frame*);
        void dup_x1(Frame*);
        void dup_x2(Frame*);
        void dup2(Frame*);
        void dup2_x1(Frame*);
        void dup2_x2(Frame*);
        void f2d(Frame*);
        void f2i(Frame*);
        void f2l(Frame*);
        void fadd(Frame*);
        void faload(Frame*);
        void fastore(Frame*);
        void fcmpg(Frame*);
        void fcpml(Frame*);
        void fconst_0(Frame*);
        void fconst_1(Frame*);
        void fconst_2(Frame*);
        void fdiv(Frame*);
        void fload(Frame*);
        void fload_0(Frame*);
        void fload_1(Frame*);
        void fload_2(Frame*);
        void fload_3(Frame*);
        void fmul(Frame*);
        void fneg(Frame*);
        void frem(Frame*);
        void freturn(Frame*);
        void fstore(Frame*);
        void fstore_0(Frame*);
        void fstore_1(Frame*);
        void fstore_2(Frame*);
        void fstore_3(Frame*);
        void fsub(Frame*);
        void getfield(Frame*);
        void getstatic(Frame*);
        void goto_(Frame*);
        void goto_w(Frame*);
        void i2b(Frame*);
        void i2c(Frame*);
        void i2d(Frame*);
        void i2f(Frame*);
        void i2l(Frame*);
        void i2s(Frame*);
        void iadd(Frame*);
        void iaload(Frame*);
        void iand(Frame*);
        void iastore(Frame*);
        void iconst_m1(Frame*);
        void iconst_0(Frame*);
        void iconst_1(Frame*);
        void iconst_2(Frame*);
        void iconst_3(Frame*);
        void iconst_4(Frame*);
        void iconst_5(Frame*);
        void idiv(Frame*);
        void if_acmpeq(Frame*);
        void if_acmpne(Frame*);
        void if_icmpeq(Frame*);
        void if_icmpne(Frame*);
        void if_icmplt(Frame*);
        void if_icmpge(Frame*);
        void if_icmpgt(Frame*);
        void if_icmple(Frame*);
        void ifeq(Frame*);
        void ifne(Frame*);
        void iflt(Frame*);
        void ifge(Frame*);
        void ifgt(Frame*);
        void ifle(Frame*);
        void ifnonnull(Frame*);
        void ifnull(Frame*);
        void iinc(Frame*);
        void iload(Frame*);
        void iload_0(Frame*);
        void iload_1(Frame*);
        void iload_2(Frame*);
        void iload_3(Frame*);
        void imul(Frame*);
        void ineg(Frame*);
        void instanceof(Frame*);
        void invokedynamic(Frame*);
        void invokeinterface(Frame*);
        void invokespecial(Frame*);
        void invokestatic(Frame*);
        void invokevirtual(Frame*);
        void ior(Frame*);
        void irem(Frame*);
        void ireturn(Frame*);
        void ishl(Frame*);
        void ishr(Frame*);
        void istore(Frame*);
        void istore_0(Frame*);
        void istore_1(Frame*);
        void istore_2(Frame*);
        void istore_3(Frame*);
        void isub(Frame*);
        void iushr(Frame*);
        void ixor(Frame*);
        void jsr(Frame*);
        void jsr_w(Frame*);
        void l2d(Frame*);
        void l2f(Frame*);
        void l2i(Frame*);
        void ladd(Frame*);
        void laload(Frame*);
        void land(Frame*);
        void lastore(Frame*);
        void lcmp(Frame*);
        void lconst_0(Frame*);
        void lconst_1(Frame*);
        void ldc(Frame*);
        void ldc_w(Frame*);
        void ldc2_w(Frame*);
        void ldiv(Frame*);
        void lload(Frame*);
        void lload_0(Frame*);
        void lload_1(Frame*);
        void lload_2(Frame*);
        void lload_3(Frame*);
        void lmul(Frame*);
        void lneg(Frame*);
        void lookupswitch(Frame*);
        void lor(Frame*);
        void lrem(Frame*);
        void lreturn(Frame*);
        void lshl(Frame*);
        void lshr(Frame*);
        void lstore(Frame*);
        void lstore_0(Frame*);
        void lstore_1(Frame*);
        void lstore_2(Frame*);
        void lstore_3(Frame*);
        void lsub(Frame*);
        void lushr(Frame*);
        void lxor(Frame*);
        void monitorenter(Frame*);
        void monitorexit(Frame*);
        void multianewarray(Frame*);
        void new_(Frame*);
        void newarray(Frame*);
        void nop(Frame*);
        void pop(Frame*);
        void pop2(Frame*);
        void putfield(Frame*);
        void putstatic(Frame*);
        void ret(Frame*);
        void return_(Frame*);
        void saload(Frame*);
        void sastore(Frame*);
        void sipush(Frame*);
        void swap(Frame*);
        void tableswitch(Frame*);
        void wide(Frame*);
    };

}