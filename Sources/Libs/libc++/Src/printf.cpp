#include "printf.h"
#include "string.h"

namespace std {

    void printf(const char* str, ...) {
        char c, cnum[20];
        StringBuilder* strBuilder = new StringBuilder();
        
        __builtin_va_list args;
        __builtin_va_start(args, str);

        while((c = *str++) != 0) {
            if(c == '%') {

                switch (c = *str++)
                {
                    case 's':
                    {
                        char* _str = __builtin_va_arg(args, char*);

                        if( _str == "" || _str == NULL)
                            _str = "(null)";

                        strBuilder->append(_str);
                        break;
                    }

                    case 'c':
                        strBuilder->append((char)__builtin_va_arg(args, char));
                        break;

                    case 'u':
                    case 'd':
                        strBuilder->append((char*)itoa(__builtin_va_arg(args, int64_t), cnum, 10));
                        break;

                    case 'x':
                        strBuilder->append((char*)itoa(__builtin_va_arg(args, int64_t), cnum, 16));
                        break;

                    case ' ':
                        strBuilder->append("% ");
                        break;

                    default:
                        break;
                }
            } else {
                strBuilder->append(c);
            }
        }

        __builtin_va_end(args);

        Printlog(strBuilder->toString());
        free(strBuilder);
    }

}
