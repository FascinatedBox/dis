#include "lily_int_opcode.h"

#include "lily_api_code_iter.h"
#include "lily_api_msgbuf.h"
#include "lily_api_value.h"

/**
package dis

This package provides utilities for disassembling a native function to get an
idea of what's going on inside.
*/

static const char *opcode_names[] =
{
    "o_fast_assign",
    "o_assign",
    "o_integer_add",
    "o_integer_minus",
    "o_modulo",
    "o_integer_mul",
    "o_integer_div",
    "o_left_shift",
    "o_right_shift",
    "o_bitwise_and",
    "o_bitwise_or",
    "o_bitwise_xor",
    "o_double_add",
    "o_double_minus",
    "o_double_mul",
    "o_double_div",
    "o_is_equal",
    "o_not_eq",
    "o_less",
    "o_less_eq",
    "o_greater",
    "o_greater_eq",
    "o_unary_not",
    "o_unary_minus",
    "o_jump",
    "o_jump_if",
    "o_integer_for",
    "o_for_setup",
    "o_foreign_call",
    "o_native_call",
    "o_function_call",
    "o_return_val",
    "o_return_noval",
    "o_build_list",
    "o_build_tuple",
    "o_build_hash",
    "o_build_enum",
    "o_get_item",
    "o_set_item",
    "o_get_global",
    "o_set_global",
    "o_get_readonly",
    "o_get_integer",
    "o_get_boolean",
    "o_new_instance_basic",
    "o_new_instance_speculative",
    "o_new_instance_tagged",
    "o_get_property",
    "o_set_property",
    "o_push_try",
    "o_pop_try",
    "o_except_ignore",
    "o_except_catch",
    "o_raise",
    "o_match_dispatch",
    "o_variant_decompose",
    "o_get_upvalue",
    "o_set_upvalue",
    "o_create_closure",
    "o_create_function",
    "o_load_class_closure",
    "o_load_closure",
    "o_dynamic_cast",
    "o_interpolation",
    "o_optarg_dispatch",
    "o_return_from_vm"
};

static void dis(lily_msgbuf *msgbuf, uint16_t *buffer, uint16_t *pos, const char *str)
{
    *pos = *pos + 1;
    char temp[64];
    sprintf(temp, "    [%4.d] %-9s%d\n", *pos, str, buffer[*pos]);
    lily_mb_add(msgbuf, temp);
}

#define ADD_FMT(...) sprintf(fmt_buffer, __VA_ARGS__); lily_mb_add(msgbuf, fmt_buffer);

#define T_VALUE   ADD_FMT("    [%4.d] value:   %d\n",  pos, (int16_t)buffer[pos]);
#define T_INPUT   ADD_FMT("    [%4.d] <------  #%d\n", pos, buffer[pos]);
#define T_OUTPUT  ADD_FMT("    [%4.d] ======>  #%d\n", pos, buffer[pos]);
#define T_JUMP    ADD_FMT("    [%4.d] +>    |  [%d] (%d)\n", pos, ci.offset + (int16_t)buffer[pos], (int16_t)buffer[pos]);
#define T_G_IN    ADD_FMT("    [%4.d] <------  #%d (G)\n", pos, buffer[pos]);
#define T_G_OUT   ADD_FMT("    [%4.d] ======>  #%d (G)\n", pos, buffer[pos]);
#define T_U_IN    ADD_FMT("    [%4.d] <------  #%d (up)\n", pos, buffer[pos]);
#define T_U_OUT   ADD_FMT("    [%4.d] ======>  #%d (up)\n", pos, buffer[pos]);

static void dump_code(lily_msgbuf *msgbuf, lily_function_val *fv)
{
    lily_code_iter ci;
    lily_ci_from_native(&ci, fv);
    char fmt_buffer[64];
    int first_pass = 1;

    while (lily_ci_next(&ci)) {
        uint16_t *buffer = ci.buffer;
        uint16_t pos = ci.offset;
        lily_opcode op = ci.buffer[pos];
        int i;

        if (first_pass == 0)
            lily_mb_add(msgbuf, "\n");
        else
            first_pass = 0;

        lily_mb_add_fmt(msgbuf, "[%d-%d] (%d) %s\n", pos,
                pos + ci.round_total - 1, buffer[pos],
                opcode_names[buffer[pos]]);

        if (ci.line)
            dis(msgbuf, buffer, &pos, "line:");

        if (ci.special_1) {
            switch (op) {
                case o_jump_if:
                    dis(msgbuf, buffer, &pos, "truthy:");
                    break;
                case o_get_integer:
                case o_get_boolean:
                    dis(msgbuf, buffer, &pos, "value:");
                    break;
                case o_set_global:
                    pos++;
                    T_G_IN
                    break;
                case o_get_global:
                    pos++;
                    T_G_OUT
                    break;
                case o_function_call:
                    pos++;
                    T_INPUT
                    break;
                case o_match_dispatch:
                    pos++;
                    T_INPUT
                    dis(msgbuf, buffer, &pos, "class:");
                    pos++;
                    break;
                case o_foreign_call:
                case o_native_call:
                    dis(msgbuf, buffer, &pos, "func:");
                    break;
                case o_build_enum:
                    dis(msgbuf, buffer, &pos, "variant:");
                    break;
                case o_dynamic_cast:
                case o_except_catch:
                case o_new_instance_basic:
                case o_new_instance_speculative:
                case o_new_instance_tagged:
                    dis(msgbuf, buffer, &pos, "class:");
                    break;
                case o_get_readonly:
                    dis(msgbuf, buffer, &pos, "literal:");
                    break;
                case o_get_property:
                case o_set_property:
                    dis(msgbuf, buffer, &pos, "index:");
                    break;
                case o_except_ignore:
                    dis(msgbuf, buffer, &pos, "class:");
                    dis(msgbuf, buffer, &pos, "pad:");
                    break;
                case o_optarg_dispatch:
                    dis(msgbuf, buffer, &pos, "start:");
                    break;
                case o_set_upvalue:
                    pos++;
                    T_U_IN
                    break;
                case o_get_upvalue:
                    pos++;
                    T_U_OUT
                    break;
                case o_create_closure:
                    dis(msgbuf, buffer, &pos, "size:");
                    break;
                default:
                    pos += ci.special_1;
                    break;
            }
        }

        if (ci.counter_2)
            dis(msgbuf, buffer, &pos, "count:");

        for (i = 0;i < ci.inputs_3;i++) {
            pos++;
            T_INPUT
        }

        if (ci.special_4) {
            switch (op) {
                case o_create_function:
                    dis(msgbuf, buffer, &pos, "literal:");
                    break;
                case o_load_closure:
                    for (i = 0;i < ci.special_4;i++) {
                        dis(msgbuf, buffer, &pos, "zap:");
                    }
                    break;
                default:
                    pos += ci.special_4;
                    break;
            }
        }

        for (i = 0;i < ci.outputs_5;i++) {
            pos++;
            T_OUTPUT
        }

        if (ci.special_6) {
            switch (op) {
                case o_foreign_call:
                case o_native_call:
                case o_function_call:
                    for (i = 0;i < ci.special_6;i++) {
                        pos++;
                        T_INPUT
                    }
                    break;
                default:
                    break;
            }
        }

        for (i = 0;i < ci.jumps_7;i++) {
            pos++;
            T_JUMP
        }
    }
}

/**
define dis(f: Function(1)): String

This receives a function that takes any number of input arguments, and returns a
`String` containing the disassembly of that function. If 'f' is not a native
function, then "<foreign function>" is returned instead.

Currently, this function only works on receiving source functions that do not
return a value. This will be fixed in a future version of Lily.
*/
void lily_dis__dis(lily_state *s)
{
    lily_msgbuf *msgbuf = lily_get_msgbuf(s);

    lily_function_val *fv = lily_arg_function(s, 0);

    if (lily_function_is_foreign(fv)) {
        lily_return_string(s, lily_new_raw_string("<foreign function>"));
        return;
    }

    dump_code(msgbuf, fv);
    lily_return_string(s, lily_new_raw_string(lily_mb_get(msgbuf)));
}

/**
define dis_rt[A](f: Function(1 => A)): String

This receives a function that takes any number of input arguments, and returns a
`String` containing the disassembly of that function. If 'f' is not a native
function, then "<foreign function>" is returned instead.

This function handles disassembly for functions that do return a value. In the
future, this function will be removed and 'dis.dis' will be usable for all
functions.
*/
void lily_dis__dis_rt(lily_state *s)
{
    lily_dis__dis(s);
}
#include "dyna_dis.h"
