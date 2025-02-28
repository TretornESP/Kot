#include "class.h"

namespace SE8 {

    Attribute** ClassParser::parseAttributes(uint16_t __attributes_count, Reader* reader) {

        if (__attributes_count == 0) return NULL;

        Attribute** ret = (Attribute**) malloc(8 * __attributes_count);

        for (uint16_t i = 0; i < __attributes_count; i++) {
            uint16_t attribute_name_index = reader->u2B();
            uint32_t attribute_length = reader->u4B();
            char* name = (char*) ((Constant_Utf8*) constant_pool[attribute_name_index])->bytes;
            if (strcmp(name, "Code")) {
                ret[i] = (Attribute*) malloc(sizeof(Attribute_Code));
                ret[i]->attribute_type = AT_Code;
                Attribute_Code* attr = (Attribute_Code*) ret[i];
                attr->max_stack = reader->u2B();
                attr->max_locals = reader->u2B();
                attr->code_length = reader->u4B();
                attr->code = reader->uL(attr->code_length);
                attr->exception_length = reader->u2B();
                attr->exception_table = (ExceptionTable**) malloc(8 * attr->exception_length);
                for (uint16_t j = 0; j < attr->exception_length; j++) {
                    ExceptionTable* entry = (ExceptionTable*) malloc(sizeof(ExceptionTable));
                    entry->start_pc = reader->u2B();
                    entry->end_pc = reader->u2B();
                    entry->handler_pc = reader->u2B();
                    entry->catch_type = reader->u2B();
                    attr->exception_table[j] = entry;
                }
                attr->attributes_count = reader->u2B();
                attr->attributes = parseAttributes(attr->attributes_count, reader);
            } else if (strcmp(name, "ConstantValue")) {
                ret[i] = (Attribute*) malloc(sizeof(Attribute_ConstantValue));
                ret[i]->attribute_type = AT_ConstantValue;
                ((Attribute_ConstantValue*) ret[i])->constantvalue_index = reader->u2B();
            } else if (strcmp(name, "StackMapTable")) {
                ret[i] = (Attribute*) malloc(sizeof(Attribute_StackMapTable));
                ret[i]->attribute_type = AT_StackMapTable;
                Attribute_StackMapTable* attr = (Attribute_StackMapTable*) ret[i];
                attr->number_of_entries = reader->u2B();
                attr->entries = (StackMapFrame**) malloc(8 * attr->number_of_entries);
                for (uint16_t j = 0; j < attr->number_of_entries; j++) {
                    attr->entries[j] = (StackMapFrame*) malloc(sizeof(StackMapFrame));
                    uint8_t frame_type = reader->u1();
                    if (frame_type < 64) {
                        attr->entries[j]->same_frame.frame_type = frame_type;
                    } else if (frame_type < 128) {
                        attr->entries[j]->same_locals_1_stack_item_frame.frame_type = frame_type;
                        attr->entries[j]->same_locals_1_stack_item_frame.stack = (verification_type_info*) reader->uB(4);
                    } else if (frame_type == 247) {
                        attr->entries[j]->same_locals_1_stack_item_frame_extended.frame_type = frame_type;
                        attr->entries[j]->same_locals_1_stack_item_frame_extended.offset_delta = reader->u2B();
                        attr->entries[j]->same_locals_1_stack_item_frame_extended.stack = (verification_type_info*) reader->uB(4);
                    } else if (frame_type < 251) {
                        attr->entries[j]->chop_frame.frame_type = frame_type;
                        attr->entries[j]->chop_frame.offset_delta = reader->u2B();
                    } else if (frame_type == 251) {
                        attr->entries[j]->same_frame_extended.frame_type = frame_type;
                        attr->entries[j]->same_frame_extended.offset_delta = reader->u2B();
                    } else if (frame_type < 255) {
                        attr->entries[j]->append_frame.frame_type = frame_type;
                        attr->entries[j]->append_frame.offset_delta = reader->u2B();
                        uint8_t stackSize = attr->entries[j]->append_frame.frame_type - 251;
                        attr->entries[j]->append_frame.stack = (verification_type_info**) malloc(8 * stackSize);
                        for (uint8_t k = 0; k < stackSize; k++) {
                            attr->entries[j]->append_frame.stack[k] = (verification_type_info*) reader->uB(sizeof(verification_type_info));
                        }
                    } else if (frame_type == 255) {
                        attr->entries[j]->full_frame.frame_type = frame_type;
                        attr->entries[j]->full_frame.offset_delta = reader->u2B();
                        attr->entries[j]->full_frame.number_of_locals = reader->u2B();
                        attr->entries[j]->full_frame.locals = (verification_type_info**) malloc(8 * attr->entries[j]->full_frame.number_of_locals);
                        for (uint8_t k = 0; k < attr->entries[j]->full_frame.number_of_locals; k++) {
                            attr->entries[j]->full_frame.locals[k] = (verification_type_info*) reader->uB(sizeof(verification_type_info));
                        }
                        attr->entries[j]->full_frame.number_of_stack_items = reader->u2B();
                        attr->entries[j]->full_frame.stack = (verification_type_info**) malloc(8 * attr->entries[j]->full_frame.number_of_stack_items);
                        for (uint8_t k = 0; k < attr->entries[j]->full_frame.number_of_stack_items; k++) {
                            attr->entries[j]->full_frame.stack[k] = (verification_type_info*) reader->uB(sizeof(verification_type_info));
                        }
                    }
                }
            } else if (strcmp(name, "Exceptions")) {
                ret[i]->attribute_type = AT_Exceptions;
                ret[i] = (Attribute*) malloc(sizeof(Attribute_Exceptions));
                Attribute_Exceptions* attr = (Attribute_Exceptions*) ret[i];
                attr->number_of_exceptions = reader->u2B();
                attr->exception_index_table = (uint16_t*) malloc(attr->number_of_exceptions * 2);
                for (uint16_t j = 0; j < attr->number_of_exceptions; j++) {
                    attr->exception_index_table[j] = reader->u2B();
                }
            } else if (strcmp(name, "InnerClasses")) {
                ret[i] = (Attribute*) malloc(sizeof(Attribute_InnerClasses));
                ret[i]->attribute_type = AT_InnerClasses;
                Attribute_InnerClasses* attr = (Attribute_InnerClasses*) ret[i];
                attr->number_of_classes = reader->u2B();
                attr->classes = (InnerClass*) malloc(attr->number_of_classes * sizeof(InnerClass));
                for (uint16_t j = 0; j < attr->number_of_classes; j++) {
                    attr->classes[j].inner_class_info_index = reader->u2B();
                    attr->classes[j].outer_class_info_index = reader->u2B();
                    attr->classes[j].inner_name_index = reader->u2B();
                    attr->classes[j].inner_class_access_flags = reader->u2B();
                }
            } else if (strcmp(name, "EnclosingMethod")) {
                ret[i] = (Attribute*) malloc(sizeof(Attribute_EnclosingMethod));
                ret[i]->attribute_type = AT_EnclosingMethod;
                Attribute_EnclosingMethod* attr = (Attribute_EnclosingMethod*) ret[i];
                attr->class_index = reader->u2B();
                attr->method_index = reader->u2B();
            } else if (strcmp(name, "Synthetic")) {
                ret[i] = (Attribute*) malloc(sizeof(Attribute));
                ret[i]->attribute_type = AT_Synthetic;
            } else if (strcmp(name, "Signature")) {
                ret[i] = (Attribute*) malloc(sizeof(Attribute_Signature));
                ret[i]->attribute_type = AT_Signature;
                ((Attribute_Signature*) ret[i])->signature_index = reader->u2B();
            } else if (strcmp(name, "SourceFile")) {
                ret[i] = (Attribute*) malloc(sizeof(Attribute_SourceFile));
                ret[i]->attribute_type = AT_SourceFile;
                ((Attribute_SourceFile*) ret[i])->sourcefile_index = reader->u2B();
            } else if (strcmp(name, "SourceDebugExtension")) {
                ret[i] = (Attribute*) malloc(sizeof(Attribute_SourceDebugExtension));
                ret[i]->attribute_type = AT_SourceDebugExtension;
                ((Attribute_SourceDebugExtension*) ret[i])->debug_extension = reader->uB(attribute_length);
            } else if (strcmp(name, "LineNumberTable")) {
                ret[i] = (Attribute*) malloc(sizeof(Attribute_LineNumberTable));
                ret[i]->attribute_type = AT_LineNumberTable;
                Attribute_LineNumberTable* attr = (Attribute_LineNumberTable*) ret[i];
                attr->line_number_table_length = reader->u2B();
                attr->line_number_table = (LineNumberTable*) malloc(sizeof(LineNumberTable) * attr->line_number_table_length);
                for (uint16_t j = 0; j < attr->line_number_table_length; j++) {
                    attr->line_number_table[j].start_pc = reader->u2B();
                    attr->line_number_table[j].line_number = reader->u2B();
                }
            } else if (strcmp(name, "Deprecated")) {
                ret[i] = (Attribute*) malloc(sizeof(Attribute));
                ret[i]->attribute_type = AT_Deprecated;
            }
            ret[i]->attribute_name_index = attribute_name_index;
            ret[i]->attribute_length = attribute_length;
        }

        return ret;
    }

    ClassParser::ClassParser(uintptr_t bytes) {

        Reader* reader = (Reader*) malloc(sizeof(Reader));
        reader->buffer = bytes;

        // header

        magic = reader->u4B();

        if (magic != 0xCAFEBABE) {
            // todo: create actual error
            Printlog("invalid file format");
            return;
        }

        minorVersion = reader->u2B();
        majorVersion = reader->u2B();

        if (majorVersion > 52) {
            // todo: create actual error
            Printlog("invalid java version");
            return;
        }

        // constant pool

        constant_pool_count = reader->u2B();

        constant_pool = (uintptr_t*) malloc(constant_pool_count*sizeof(uintptr_t));

        for (uint16_t i = 1; i < constant_pool_count; i++) {
            uint8_t tag = reader->u1();
            uintptr_t entry;
            if (tag == CONSTANT_Class) {
                uint16_t name_index = reader->u2B();
                entry = (uintptr_t) malloc(sizeof(Constant_ClassInfo));
                ((Constant_ClassInfo*) entry)->name_index = name_index;
            } else if (tag == CONSTANT_Utf8) {
                uint16_t length = reader->u2B();
                uint8_t* bytes = reader->uL(length);
                entry = (uintptr_t) malloc(sizeof(Constant_Utf8));
                ((Constant_Utf8*) entry)->length = length;
                ((Constant_Utf8*) entry)->bytes = bytes;
            } else if (tag == CONSTANT_NameAndType) {
                uint16_t name_index = reader->u2B();
                uint16_t descriptor_index = reader->u2B();
                entry = (uintptr_t) malloc(sizeof(Constant_NameAndType));
                ((Constant_NameAndType*) entry)->name_index = name_index;
                ((Constant_NameAndType*) entry)->descriptor_index = descriptor_index;
            } else if (tag == CONSTANT_String) {
                uint16_t string_index = reader->u2B();
                entry = (uintptr_t) malloc(sizeof(Constant_String));
                ((Constant_String*) entry)->string_index = string_index;
            } else if (tag == CONSTANT_Integer || tag == CONSTANT_Float) {
                uint32_t bytes = reader->u4B();
                entry = (uintptr_t) malloc(sizeof(Constant_Integer_Float));
                ((Constant_Integer_Float*) entry)->bytes = bytes;
            } else if (tag == CONSTANT_Double || tag == CONSTANT_Long) {
                uint32_t high_bytes = reader->u4B();
                uint32_t low_bytes = reader->u4B();
                entry = (uintptr_t) malloc(sizeof(Constant_Double_Long));
                ((Constant_Double_Long*) entry)->high_bytes = high_bytes;
                ((Constant_Double_Long*) entry)->low_bytes = low_bytes;
            } else if (tag == CONSTANT_Fieldref || tag == CONSTANT_Methodref || tag == CONSTANT_InterfaceMethodref) {
                uint16_t class_index = reader->u2B();
                uint16_t name_and_type_index = reader->u2B();
                entry = (uintptr_t) malloc(sizeof(Constant_RefInfo));
                ((Constant_RefInfo*) entry)->class_index = class_index;
                ((Constant_RefInfo*) entry)->name_and_type_index = name_and_type_index;
            } else if (tag == CONSTANT_MethodHandle) {
                uint8_t reference_kind = reader->u1();
                uint16_t reference_index = reader->u2B();
                entry = (uintptr_t) malloc(sizeof(Constant_MethodHandle));
                ((Constant_MethodHandle*) entry)->reference_kind = reference_kind;
                ((Constant_MethodHandle*) entry)->reference_index = reference_index;
            } else if (tag == CONSTANT_MethodType) {
                uint16_t descriptor_index = reader->u2B();
                entry = (uintptr_t) malloc(sizeof(Constant_Type));
                ((Constant_Type*) entry)->descriptor_index = descriptor_index;
            } else if (tag == CONSTANT_InvokeDynamic) {
                uint16_t bootstrap_method_attr_index = reader->u2B();
                uint16_t name_and_type_index = reader->u2B();
                entry = (uintptr_t) malloc(sizeof(Constant_InvokeDynamic));
                ((Constant_InvokeDynamic*) entry)->bootstrap_method_attr_index = bootstrap_method_attr_index;
                ((Constant_InvokeDynamic*) entry)->name_and_type_index = name_and_type_index;
            } else {
                // todo: create actual error
                Printlog("unsupported constant pool tag");
                return;
            }
            ((ConstantPoolEntry*) entry)->tag = tag;
            constant_pool[i] = entry;
        }   

        access_flags = reader->u2B();
        this_class = reader->u2B();
        super_class = reader->u2B();

        // interfaces

        interfaces_count = reader->u2B();
        interfaces = (uint16_t*) malloc(interfaces_count*2);

        for (uint16_t i = 0; i < interfaces_count; i++) {
            uint16_t index = reader->u2B();
            interfaces[i] = index;
        }

        // fields

        fields_count = reader->u2B();
        fields = (FieldInfo**) malloc(sizeof(FieldInfo*)*fields_count);

        for (uint16_t i = 0; i < fields_count; i++) {
            fields[i] = (FieldInfo*) malloc(sizeof(FieldInfo));
            fields[i]->access_flags = reader->u2B();
            fields[i]->name_index = reader->u2B();
            fields[i]->descriptor_index = reader->u2B();
            fields[i]->attributes_count = reader->u2B();
            fields[i]->attributes = parseAttributes(attributes_count, reader);
        }

        // methods

        methods_count = reader->u2B();
        methods = (Method**) malloc(sizeof(Method*)*methods_count);

        for (uint16_t i = 0; i < methods_count; i++) {
            methods[i] = (Method*) malloc(sizeof(Method));
            methods[i]->access_flags = reader->u2B();
            methods[i]->name_index = reader->u2B();
            methods[i]->descriptor_index = reader->u2B();
            methods[i]->attributes_count = reader->u2B();
            methods[i]->attributes = parseAttributes(methods[i]->attributes_count, reader);
        }

        // attributes

        attributes_count = reader->u2B();
        attributes = parseAttributes(attributes_count, reader);

        free(reader);

    }

    // char* ClassParser::getSourceFileName() {
    //     for (uint16_t i = 0; i < attributes_count; i++) {
    //         Attribute* attr = (Attribute*) attributes[i];
    //         char* name = (char*) ((Constant_Utf8*) constant_pool[attr->attribute_name_index])->bytes;
    //         if (strcmp(name, "SourceFile")) {
    //             return (char*) ((Constant_Utf8*) constant_pool[((Attribute_SourceFile*) attr)->sourcefile_index])->bytes;
    //         }
    //     }
    //     return NULL;
    // }

    // Frame* ClassParser::getEntryPoint(JavaVM* jvm) {
    //     for (uint16_t i = 0; i < methods_count; i++) {
    //         Method* method = methods[i];
    //         if (AF_isStatic(method->access_flags) && AF_isPublic(method->access_flags)) {
    //             if (strcmp((char*)((Constant_Utf8*)constant_pool[method->name_index])->bytes, "main")) {
    //                 Frame* frame = (Frame*) malloc(sizeof(Frame));
    //                 frame->init(jvm, this, method);
    //                 return frame;
    //             }
    //         }
    //     } 
    //     return NULL;
    // }

    uint8_t ClassParser::getStaticFieldSize(char* fieldName) {
        for (uint16_t i = 0; i < fields_count; i++) {
            if (AF_isStatic(fields[i]->access_flags)) {
                if (strcmp((char*)((Constant_Utf8*)constant_pool[fields[i]->name_index])->bytes, fieldName)) {
                    char* desc = (char*) ((Constant_Utf8*) constant_pool[fields[i]->descriptor_index])->bytes;
                    switch(desc[0]) {
                        case 'B':
                        case 'Z':
                            return 1;
                        case 'C':
                        case 'S':
                            return 2;
                        case 'L':
                        case '[':
                        case 'I':
                        case 'F':
                            return 4;
                        case 'D':
                        case 'J':
                            return 8;
                        default:
                            return 0;
                    }
                }
            }
        }
        return 0;
    }

    uint8_t ClassParser::getFieldSize(char* fieldName) {
        for (uint16_t i = 0; i < fields_count; i++) {
            if (!AF_isStatic(fields[i]->access_flags)) {
                if (strcmp((char*)((Constant_Utf8*)constant_pool[fields[i]->name_index])->bytes, fieldName)) {
                    char* desc = (char*) ((Constant_Utf8*) constant_pool[fields[i]->descriptor_index])->bytes;
                    switch(desc[0]) {
                        case 'B':
                        case 'Z':
                            return 1;
                        case 'C':
                        case 'S':
                            return 2;
                        case 'L':
                        case '[':
                        case 'I':
                        case 'F':
                            return 4;
                        case 'D':
                        case 'J':
                            return 8;
                        default:
                            return 0;
                    }
                }
            }
        }
        return 0;
    }

}
