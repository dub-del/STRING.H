#include "s21_string.h"

int s21_isDigit(int character) {
    if (character >= 48 && character <= 57)
        return (1);
    else
        return (0);
}

char *s21_strdup(const char *s1) {
    char *s2;
    s21_size_t len;
    s21_size_t i;

    len = s21_strlen(s1);
    if (!(s2 = (char *)malloc(sizeof(char) * len + 1))) return (s21_NULL);
    i = 0;
    while (s1[i]) {
        s2[i] = s1[i];
        i += 1;
    }
    s2[i] = '\0';
    return (s2);
}

int helper_isSpec(char c, struct s_format *formatParams) {
    int i = 0;
    if (c == 'd' || c == 'i' || c == 'c' || c == 'f' || c == 's' || c == 'u' ||
        c == '%') {
        i = 1;
    }
    formatParams->spec = (c == 'u' ? 1 : 0);
    return i;
}

//Инициализатор структуры
void helper_initStruct(struct s_format *formatParams) {
    formatParams->isInt = 0;
    formatParams->flagPlus = 0;
    formatParams->flagMinus = 0;
    formatParams->flagSpace = 0;
    formatParams->width = 0;
    formatParams->widthMustBeWrittenByZero = 0;
    formatParams->widthZeroDetected = 0;
    formatParams->precision = 0;
    formatParams->precisionWillBeSpecified = 0;
    formatParams->length_h = 0;
    formatParams->length_l = 0;
    formatParams->isAsteriskWidth = 0;
    formatParams->isAsteriskFloatWidthDescription = 0;
    formatParams->precisionIsNull = 0;
    formatParams->precisionSetByZero = 0;
    formatParams->isLongDouble = 0;
    formatParams->spec = 0;
}

//Записывает символ в буффер
void helper_writeChar(const char **format, char **str,
                      struct s_format *formatParams, char c) {
    if (formatParams->flagMinus) {
        **str = c;
        (*str)++;
    }
    for (int i = 0; i < formatParams->width - 1; i++) {
        **str = ' ';
        (*str)++;
    }
    if (!formatParams->flagMinus) {
        **str = c;
        (*str)++;
    }
    (*format)++;
}

//Записывает знак процента в буффер
void helper_writePercent(const char **format, char **str) {
    **str = '%';
    (*str)++;
    (*format)++;
}

//Булин положительности
void helper_isNegative(int *n, int *neg) {
    if (*n < 0) {
        *n *= -1;
        *neg = 1;
    }
}

//Заполняет строку размером n терминаторами.
void helper_bZero(void *s, s21_size_t n) {
    s21_size_t i;

    i = 0;
    while (i < n) {
        ((char *)s)[i] = 0;
        i++;
    }
}

//Выделяет память для чего-либо
void *helper_memalloc(s21_size_t size) {  //БОЛЬШЕ ОДНОГО ВЫХОДА
    void *some;

    some = malloc(size);
    if (!some) return (s21_NULL);
    helper_bZero(some, size);
    return (some);
}

//Конвертирует интеджер в строку
char *helper_itoa(int n) {  //БОЛЬШЕ ОДНОГО ВЫХОДА
    int tmpn;
    int len;
    int neg;
    char *str;

    tmpn = n;
    len = 2;
    neg = 0;
    helper_isNegative(&n, &neg);

    while (tmpn /= 10) len++;
    if (!(str = helper_memalloc(len))) return (s21_NULL);
    len += neg;
    str[--len] = '\0';
    while (len--) {
        str[len] = n % 10 + '0';
        n = n / 10;
    }
    if (neg) str[0] = '-';
    return (str);
}

char *helper_utoa(unsigned int n) {  //БОЛЬШЕ ОДНОГО ВЫХОДА
    unsigned int tmpn;
    int len;
    char *str;

    tmpn = n;
    len = 2;

    while (tmpn /= 10) len++;
    if (!(str = helper_memalloc(len))) return (s21_NULL);
    str[--len] = '\0';
    while (len--) {
        str[len] = n % 10 + '0';
        n = n / 10;
    }
    return (str);
}

char *helper_ldtoa(long int n) {  //БОЛЬШЕ ОДНОГО ВЫХОДА
    long int tmpn;
    int len;
    // int        neg;
    char *str;

    tmpn = n;
    len = 2;
    // neg = 0;
    // helper_isNegative(&n, &neg);

    while (tmpn /= 10) len++;
    if (!(str = helper_memalloc(len))) return (s21_NULL);
    // len += neg;
    str[--len] = '\0';
    while (len--) {
        str[len] = n % 10 + '0';
        n = n / 10;
    }
    // if (neg)
    //     str[0] = '-';
    return (str);
}

char *checkFloatMaxMinCases(long double f_case,
                            char *str)  //БОЛЬШЕ ОДНОГО ВЫХОДА
{
    if (f_case == INFINITY) return (s21_strdup("inf"));
    if (f_case == -INFINITY) return (s21_strdup("-inf"));
    if (f_case == DBL_MAX || f_case == FLT_MAX)
        helper_floatMaxConversion(str);
    else
        free(str);
    char *s;
    s = helper_strNew(2);
    return (s);
}

//Конвертирует флоат в строку
char *helper_ftoa(long double f,
                  struct s_format formatParams)  //БОЛЬШЕ ОДНОГО ВЫХОДА
{
    int i;
    double fpNumber;
    int size;
    char *str = "";
    int isNegative;

    i = 0;
    size = 1;
    isNegative = 0;
    fpNumber = f;

    if (fpNumber == INFINITY || fpNumber == -INFINITY || fpNumber == DBL_MAX ||
        fpNumber == FLT_MAX) {
        checkFloatMaxMinCases(fpNumber, str);
    }
    if (fpNumber < 0) isNegative = 1;

    char *lenNum = (helper_itoa(fpNumber));

    size += (s21_strlen(lenNum) + isNegative);
    free(lenNum);
    size += formatParams.precision <= 0 ? 6 : formatParams.precision;
    size = size < 0 ? 6 : size;
    str = helper_strNew(size + 4);

    if (isNegative == 1) str[i++] = '-';

    helper_convertInteger(&fpNumber, &str, &i,
                          (helper_findMultiplier(fpNumber, &size)));

    if ((formatParams.precision == 0 &&
         formatParams.precisionWillBeSpecified == 1) ||
        formatParams.precisionIsNull) {
        helper_cutStr(str, formatParams.precision, 1);
        return (str);
    }
    helper_convertDecimal(&str, &i, (float)fpNumber, size);
    str = helper_round(str, formatParams.precision, 1);
    helper_cutStr(str, formatParams.precision, 0);
    return (str);
}

//Конвертирует максимальный флоат
char *helper_floatMaxConversion(char *str)  //БОЛЬШЕ ОДНОГО ВЫХОДА
{
    int i;
    char float_max[50];

    i = 0;
    s21_strcpy(float_max, "340282346638528859811704183484516925440.000000\0");
    if (!(str = (char *)malloc(sizeof(float_max)))) return (NULL);
    while (float_max[i] != '\0') {
        str[i] = float_max[i];
        i++;
    }
    return (str);
}
//Ищет множитель для работы с десятичной частью
long long int helper_findMultiplier(long double fpNumber, int *size) {
    long int multiplier;
    long int extra;

    extra = (long int)(fpNumber);
    multiplier = 1;
    while ((extra /= 10) != 0) {
        multiplier *= 10;
        size++;
    }
    return (multiplier);
}

//Конвертирует десятичную часть дабла
void helper_convertDecimal(char **str, int *i, float fpNumber, int accuracy) {
    int j;
    long long int tmp;
    char *s;

    fpNumber *= 10;
    j = 0;
    s = *str;
    s[(*i)++] = '.';
    while (j++ < accuracy) {
        tmp = ((long int)fpNumber != 9) ? (long int)(fpNumber + 0.01)
                                        : (long int)fpNumber;
        s[(*i)++] = (char)(tmp + 48);
        fpNumber = (fpNumber - tmp) * 10;
    }
}

//Конвертирует целую часть дабла
void helper_convertInteger(double *fpNumber, char **str, int *i,
                           long int multiplier) {
    char *s;
    long int b;

    b = (long int)*fpNumber;
    s = *str;
    if (b < 0) {
        *fpNumber *= -1;
        b *= -1;
    }
    if (b == 0) {
        s[(*i)++] = '0';
        return;
    }
    *fpNumber -= (long double)b;
    while (multiplier >= 1) {
        s[(*i)++] = (char)((b / multiplier) + 48);
        b = b - (b / multiplier) * multiplier;
        multiplier /= 10;
    }
}

//Режет строку согласно необходимой точности
void helper_cutStr(char *str, int pres, int isSetByZero) {
    int i;
    int k;

    i = 0;
    if (isSetByZero) {
        k = 0;
    } else {
        k = pres <= 0 ? 6 : pres;
        while (str[i] != '.') str++;
    }

    while (k != 0 && *str) {
        str++;
        k--;
    }
    i -= isSetByZero ? 1 : 0;

    while (str[i++] != '\0') str[i] = '\0';
}

//Создает новую строку
char *helper_strNew(s21_size_t size)  //БОЛЬШЕ ОДНОГО ВЫХОДА
{
    char *str;

    if (size + 1 < size) return (NULL);
    str = (char *)malloc(size + 1);
    if (str == 0) return (NULL);
    helper_bZero(str, size + 1);
    return (str);
}

//Удаляет строку, вычищая память
void helper_strdel(char **as) {
    if (as) {
        free(*as);
        *as = s21_NULL;
    }
}

//Возвращает длину предполагаемой строки исходя из количество цифр в числе
int helper_countNumerals(long double chislo) {
    char *str;
    int k;

    str = helper_itoa((long long int)chislo);
    k = s21_strlen(str);
    // helper_strdel(&str);

    return (k);
}

//Конвертирует дабл
char *helper_dtoa(long double ldNumber,
                  struct s_format formatParams) {  //БОЛЬШЕ ОДНОГО ВЫХОДА
    int i;
    long double chislo;
    int size;
    char *str;
    int sign;

    chislo = ldNumber;
    i = 0;
    size = 1;
    sign = 0;
    if (chislo < 0) sign = 1;
    size += formatParams.precision <= 0 ? 6 : formatParams.precision;
    size += (100 + sign);
    str = helper_strNew(helper_countNumerals(chislo) + 1);
    if (sign == 1) str[i++] = '-';
    helper_convertDblInteger(&chislo, &str, &i,
                             (helper_findMultiplier(chislo, &size)));
    if (formatParams.precision == 0 &&
        formatParams.precisionWillBeSpecified == 1) {
        helper_cutStr(str, formatParams.precision,
                      formatParams.precisionSetByZero);
        return (str);
    }
    helper_convertDblDecimal(&str, &i, chislo,
                             (helper_countNumerals(chislo) + size + 4 + sign));
    str = helper_round(str, formatParams.precision,
                       formatParams.precisionSetByZero);
    helper_cutStr(str, formatParams.precision, formatParams.precisionSetByZero);
    str[i - 1] = '\0';
    return (str);
}

//Конвертирует целую часть дабла
void helper_convertDblDecimal(char **str, int *i, long double chislo,
                              int accuracy) {
    int j;
    int tmp;
    char *s;

    chislo *= 10;
    j = 0;
    s = *str;
    s[(*i)++] = '.';
    while (j++ < accuracy) {
        tmp = ((long long int)chislo != 9) ? (long long int)(chislo + 0.01)
                                           : (long long int)chislo;
        s[(*i)++] = (char)(tmp + 48);
        chislo = (chislo - tmp) * 10;
    }
}

//Конвертирует десятичную часть дабла
void helper_convertDblInteger(long double *ldblNumber, char **str, int *i,
                              long int multiplier) {
    char *s;
    long int b;

    b = (long int)*ldblNumber;
    s = *str;
    if (b < 0) {
        *ldblNumber *= -1;
        b *= -1;
    }
    if (b == 0) s[(*i)++] = '0';
    *ldblNumber -= (long double)b;
    while (multiplier >= 1) {
        s[(*i)++] = (char)((b / multiplier) + 48);
        b = b - (b / multiplier) * multiplier;
        multiplier /= 10;
    }
}

//Округлятор последней цифры
char *helper_round(char *str, int accuracy, int precisionSetByZero) {
    int i = 0;

    while (str[i] != '.') {
        i++;
    }
    if (accuracy == 0 && precisionSetByZero) {
        accuracy = 0;
    } else if (accuracy == 0) {
        accuracy = 6;
    }
    i += accuracy + 1;
    if (str[i] >= '5') {
        str[--i] += 1;
    }
    return (str);
}

void handlePrec(char *buff, struct s_format *formatParams) {
    char tmp[BUFFSIZE] = {'\0'};
    int isNeg = 0;
    int len = s21_strlen(buff);

    if (buff[0] == '-') {
        tmp[0] = '-';
        len--;
        isNeg = 1;
    }

    if (formatParams->precision > len) {
        int i;
        for (i = isNeg; i < formatParams->precision - len + isNeg; i++)
            tmp[i] = '0';

        for (int j = isNeg; buff[j]; i++, j++) tmp[i] = buff[j];

        s21_strcpy(buff, tmp);
    }
    if (formatParams->precisionWillBeSpecified &&
        formatParams->precision == 0 && formatParams->isInt && buff[0] == '0')
        buff[0] = '\0';
};

void handleFlags(char *buff, struct s_format *formatParams) {
    char tmp[BUFFSIZE + 1] = {'\0'};
    if (formatParams->flagPlus && !formatParams->spec) {
        tmp[0] = buff[0] == '-' ? buff[0] : '+';
        s21_strcpy(tmp + 1, buff[0] == '-' ? buff + 1 : buff);
        s21_strcpy(buff, tmp);
    } else if (formatParams->flagSpace && buff[0] != '-' &&
               formatParams->spec != 1) {
        tmp[0] = ' ';
        s21_strcpy(tmp + 1, buff);
        s21_strcpy(buff, tmp);
    }
    if (formatParams->width > (int)s21_strlen(buff)) {
        int i = formatParams->width - s21_strlen(buff);
        if (!formatParams->flagMinus) {
            s21_memset(tmp, formatParams->widthMustBeWrittenByZero ? '0' : ' ',
                       i);
            s21_strcpy(tmp + i, buff);
        } else {
            s21_strcpy(tmp, buff);
            s21_memset(tmp + s21_strlen(tmp), ' ', i);
        }
        s21_strcpy(buff, tmp);
    }
}

void writeInt(char *buff, struct s_format *formatParams, char *string) {
    s21_memcpy(buff, string, s21_strlen(string));
    handlePrec(buff, formatParams);
    handleFlags(buff, formatParams);
}

void handleString(struct s_format *formatParams, char *buff, char *str) {
    char tmp[BUFFSIZE] = {'\0'};
    s21_strcpy(tmp, str);
    if (formatParams->precisionWillBeSpecified)
        tmp[formatParams->precision] = '\0';

    int shift = formatParams->width - s21_strlen(tmp);
    int len = s21_strlen(tmp);
    if (formatParams->precisionIsNull) s21_memset(tmp, ' ', len);
    if (formatParams->flagMinus && shift > 0) {
        s21_strcpy(buff, tmp);
        s21_memset(buff + len, ' ', shift);
    } else if (shift > 0) {
        s21_memset(buff, ' ', shift);
        s21_strcpy(buff + shift, tmp);
    } else {
        s21_strcpy(buff, tmp);
    }
}

void handleWideString(struct s_format *formatParams, char *buff,
                      wchar_t *wstr) {
    char tmp[BUFFSIZE] = {'\0'};
    char str[BUFFSIZE] = {'\0'};

    wcstombs(str, wstr, BUFFSIZE);
    s21_strcpy(tmp, str);
    if (formatParams->precisionWillBeSpecified)
        tmp[formatParams->precision] = '\0';

    int shift = formatParams->width - s21_strlen(tmp);
    int len = s21_strlen(tmp);

    if (formatParams->flagMinus && shift > 0) {
        s21_strcpy(buff, tmp);
        s21_memset(buff + len, ' ', shift);
    } else if (shift > 0) {
        s21_memset(buff, ' ', shift);
        s21_strcpy(buff + shift, tmp);
    } else {
        s21_strcpy(buff, tmp);
    }
}

void writeFloat(char *buff, struct s_format *formatParams, char *string) {
    s21_memcpy(buff, string, s21_strlen(string));
    handleFlags(buff, formatParams);
}

int parser(const char **inputLine, struct s_format *inputLineParams) {
    int flags = 0;

    while (**inputLine) {
        if (**inputLine == '-' || **inputLine == '+' || **inputLine == ' ') {
            if (**inputLine == '-')
                inputLineParams->flagMinus = 1;
            else if (**inputLine == '+')
                inputLineParams->flagPlus = 1;
            else if (**inputLine == ' ')
                inputLineParams->flagSpace = 1;
            (*inputLine)++;
        } else if (s21_isDigit(**inputLine) == 1) {
            if (**inputLine == '0' && inputLineParams->widthZeroDetected == 0) {
                (*inputLine)--;
                if (s21_isDigit(**inputLine) == 0) {
                    inputLineParams->widthMustBeWrittenByZero = 1;
                    inputLineParams->widthZeroDetected = 1;
                }
                (*inputLine)++;
            }
            inputLineParams->width =
                **inputLine - '0' + inputLineParams->width * 10;
            (*inputLine)++;
        } else if (**inputLine == '*') {
            (*inputLine)--;
            if (**inputLine != '.') {
                inputLineParams->isAsteriskWidth = 1;
                (*inputLine)++;
            } else
                (*inputLine)++;
            (*inputLine)++;
        } else if (**inputLine == '.') {
            (*inputLine)++;
            if ((!s21_isDigit(**inputLine)) && **inputLine != '*')
                inputLineParams->precisionIsNull = 1;
            if (**inputLine == '*') {
                inputLineParams->isAsteriskFloatWidthDescription = 1;
                inputLineParams->precisionWillBeSpecified = 1;
            } else if (**inputLine == 'L' || **inputLine == 'f' ||
                       s21_isDigit(**inputLine) == 1)
                inputLineParams->precisionWillBeSpecified = 1;
            else if (**inputLine == '0')
                inputLineParams->precisionSetByZero = 1;
            while (s21_isDigit(**inputLine)) {
                inputLineParams->precision =
                    **inputLine - '0' + inputLineParams->precision * 10;
                (*inputLine)++;
            }
        } else if (**inputLine == 'h' || **inputLine == 'l' ||
                   **inputLine == 'L') {
            if (**inputLine == 'h')
                inputLineParams->length_h = 1;
            else if (**inputLine == 'l')
                inputLineParams->length_l = 1;
            else if (**inputLine == 'L')
                inputLineParams->isLongDouble = 1;
            (*inputLine)++;
        } else if (helper_isSpec(**inputLine, inputLineParams) == 1) {
            flags = 1;
            break;
        } else {
            flags = 0;
            break;
        }
    }
    return flags;
}

void helper_writeInBuffer(const char **format, char **str,
                          struct s_format *formatParams, va_list vlist,
                          char *buff) {
    if (**format == 'c') {
        if (formatParams->isAsteriskWidth) {
            formatParams->width = va_arg(vlist, int);
        }
        char c = va_arg(vlist, int);
        helper_writeChar(format, str, formatParams, c);
    } else if (**format == 'L') {
        formatParams->isLongDouble = 1;
    } else if (**format == '%') {
        if (formatParams->isAsteriskWidth) {
            formatParams->width = va_arg(vlist, int);
        }
        helper_writePercent(format, str);
    } else if (**format == 's') {
        if (formatParams->isAsteriskWidth) {
            formatParams->width = va_arg(vlist, int);
        }
        if (formatParams->isAsteriskFloatWidthDescription) {
            formatParams->precision = va_arg(vlist, int);
        }
        if (formatParams->length_l) {
            wchar_t *string = va_arg(vlist, wchar_t *);
            handleWideString(formatParams, buff, string);
            (*format)++;
        } else {
            char *string = va_arg(vlist, char *);
            handleString(formatParams, buff, string);
            (*format)++;
        }
    } else if (**format == 'd' || **format == 'i') {
        formatParams->isInt = 1;
        if (formatParams->isAsteriskWidth)
            formatParams->width = va_arg(vlist, int);
        if (formatParams->isAsteriskFloatWidthDescription)
            formatParams->precision = va_arg(vlist, int);
        if (formatParams->length_l) {
            long int lint = va_arg(vlist, long int);
            if (lint < 0) formatParams->flagPlus = 0;
            char *temp = helper_ldtoa(lint);
            writeInt(buff, formatParams, temp);
            free(temp);
            (*format)++;
        } else if (formatParams->length_h) {
            short int sint = va_arg(vlist, int);
            if (sint < 0) formatParams->flagPlus = 0;
            char *temp = helper_itoa(sint);
            writeInt(buff, formatParams, temp);
            free(temp);
            (*format)++;
        } else {
            int intParam = va_arg(vlist, int);
            if (intParam < 0) formatParams->flagPlus = 0;
            char *temp = helper_itoa(intParam);
            writeInt(buff, formatParams, temp);
            free(temp);
            (*format)++;
        }
    } else if (**format == 'f') {
        if (formatParams->isAsteriskWidth)
            formatParams->width = va_arg(vlist, int);
        if (formatParams->isAsteriskFloatWidthDescription) {
            if (!formatParams->precisionIsNull)
                formatParams->precision = va_arg(vlist, int);
            else
                formatParams->precision = 0;
        }
        if (formatParams->isLongDouble) {
            long double floatParam = va_arg(vlist, long double);
            writeFloat(buff, formatParams,
                       helper_dtoa(floatParam, *formatParams));
            (*format)++;
        } else {
            double floatParam = va_arg(vlist, double);
            writeFloat(buff, formatParams,
                       helper_ftoa(floatParam, *formatParams));
            (*format)++;
        }
    } else if (**format == 'u') {
        if (formatParams->isAsteriskWidth)
            formatParams->width = va_arg(vlist, int);
        if (formatParams->isAsteriskFloatWidthDescription) {
            if (!formatParams->precisionIsNull)
                formatParams->precision = va_arg(vlist, int);
            else
                formatParams->precision = 0;
        }
        unsigned int uint = (unsigned)va_arg(vlist, unsigned int);
        char *temp = helper_utoa(uint);
        writeInt(buff, formatParams, temp);
        free(temp);
        (*format)++;
    }
}
