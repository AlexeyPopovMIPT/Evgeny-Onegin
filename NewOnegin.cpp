#include <stdio.h>
#include <stdlib.h>
#include <sys\stat.h>
#include <assert.h>
#include <time.h>
#include "Stopwatch.h"


const char* INPUT  = "input.txt";
const char* OUTPUT = "output.txt";
//TODO: in command prompt (сделано)

const char* LOGO = "Sorting strings from file\nAuthor: AlexeyPopovMIPT, 2020\nVersion 1.0.0.1\n\n";

int sort_by_ru_chars = 1;
int sort_by_en_chars = 1;
int   sort_by_digits = 0;

enum Stopwatch_args {
    SW_START = 0,
      SW_END = 1
};

/*!
    Determines a unsigned char* that is located somewhere in memory
    \param start - pointer to the 1st byte of char*
    \param end - pointer to the last byte of char*
*/
struct line {

    unsigned char* start;
    unsigned char* end;

};

/*!
    Evgeny Onegin, or The enchanting strings sorting.
    The Phylologic Drama by
    \author AlexeyPopovMIPT
    \version 1.0.0.1

    Program for sorting the strings from the .txt file, whose full path is located by the INPUT address.
    As a result, application writes sorted strings, then sorted by the end strings, then original text
    to the file, whose full path is located by the OUTPUT address. These parts are separated by 50 "stars" each one on a new line.

    \note    No one Pushkins were damaged.

    \warning coding of the input must be UTF-8

    Program sorts strings by English and Russian letters (by default), so that:
    \arg Any digit is "less" than any English letter, and any English letter is "less" than any Russian letter;
    \arg In both languages any uppercase letter is "less" than any lowercase letter;
    \arg In both languages letters of both case is ordered same as in the alphabet.

    Command prompt arguments:
    <ul>
    <li> nl - do not display program info </li>
    <li> t  - display executing time </li>
    <li> rm - not to sort by Russian chars </li>
    <li> em - not to sort by English chars </li>
    <li> dp - sort by digits </li>
    </ul>


*/
int main(int argcount, char* argsarray[]);

/*!
    A direct comparator for two lines
    \param [IN] val1 - void-type pointer for the 1st line
    \param [IN] val2 - void-type pointer for the 2nd line
*/
int DirCompareTo(const void* val1, const void* val2);

/*!
    A reverse comparator for two lines
    \param [IN] val1 - void-type pointer for the 1st lines
    \param [IN] val2 - void-type pointer for the 2nd lines
*/
int RevCompareTo(const void* val1, const void* val2);

/*!
    This is comparator that compares \c str1 and \c str2 by their "valid chars", that means Russian and
        English characters and digits.
    \param [IN] str1 - 1st line
    \param [IN] str2 - 2nd line
    \param [IN] dir  - direction of comparing (1 if direct and -1 if reverse)

    \returns positive value if \c str1 is bigger than \c str2, 0 if they're equal, and negative value
        if \c str2 is bigger than \c str1.
*/
int dct_strcmp(struct line str1, struct line str2, int dir);

/*!
    This function converts a character \c *ch, which is by the address \c ch, into integer \c a, so that:

    \b if \c *ch is English letter or digit and EN_LRS is 1, \c a is ASCII code of \c *ch.

    \b else \b if \c *ch is Russian letter and RU_LRS is 1, \c a is:
        \arg UTF-8 code of \c *ch    if \c *ch is between А and Е
        \arg UTF-8 code of     Е  +1 if \c *ch is Ё
        \arg UTF-8 code of \c *ch +1 if \c *ch is between Ж and Я or between а and е
        \arg UTF-8 code of     е  +2 if \c *ch is ё
        \arg UTF-8 code of \c *ch +2 if \c *ch is between ж and я

    \b else \c a is minus size of character in UTF-8 in bytes

    As a result, for any two letters/digits A1 and A2 (A1>A2) → (code of A1 > code of A2)
    \b Note: you always can determine a size of \c *ch with given return of function. If return is negative,
    its absolute value is a count of bytes. If return is positive, count of bytes is equal to count of the
    significant bytes in return.

    \param [IN] ch  - address of edge byte of character
    \param [IN] dir - 1 if ch is address of left byte and -1 if right

    \return value of \c a

*/
int GetCharCode(unsigned char* ch, int dir);

/*!
    This function is copying data from \c Source to array \c data, removing '\r's and replacing
    '\n's by '\0's, and counting strings in \c Source.

    \warning you have to be sure that array \c data is big enough to accomodate all bytes from \c Source.

    \attention if string is empty, it will not be met in array \c data
    \attention result of counting doesn't depend on presence of \n after the last string.

    \returns count of strings
*/
int GetDataAndCountStrings(FILE* Source, unsigned char* data, int FileSize);

/*!
    Filling the array \c pointers with pointers of every new string in \c data

    \warning You have to be sure that array \c pointers is big enough
*/
void GetNewStringsPointers(unsigned char* data, struct line* pointers);

/*!
    Sorting char** \c array from element \a 0 to \a size-1 with comparator \b int \b dir_strcmp(char*,char*)
*/
int MergeSort(struct line* array, int size, int dir);

/*!
    Prints \c size lines from \c array to \c out, then 50 "stars", each one on a new line
*/
void fPrintLines(FILE* out, struct line* array, int size);

/*!
    Prints every char from \c array to \c out
    \attention \c array must have \c EOF (255) at the end
*/
void fPrintStrings(FILE* out, unsigned char* array);

/*!
    Opens file by \c filename path in \mode mode, in case of failure prints error message
    \returns 0 if successful, fopen_s otherwise
*/
int OpenFile(const char* filename, const char* mode, FILE** stream);

int GetFileSize(const char* filename);

int main(int argcount, char *argsarray[])
{
    int measure_time = 0;

    //Processing arguments
    {
        int prev_param = 0;
        int      param = 0;
        int     nologo = 0;

        //TODO: NOT CHANGE VALUES OF argc AND args (сделано)
        int argc = argcount;
        const char** args = (const char**) argsarray;
        if (argc < 3) {
            printf("%s%s",LOGO,"Pass input file path as 2nd argument, then output file path");
            return 0; 
        }
        INPUT  = args[1];
        OUTPUT = args[2];
        argc -= 2; 
        args += 2;

        while (--argc > 0 && (*++args)[0] == '-') {
            while (param = *++args[0]) {
                switch (param) {
                case 'p':
                    if (prev_param == 'd') sort_by_digits = 1;
                    break;

                case 'm':
                    if      (prev_param == 'r') sort_by_ru_chars = 0;
                    else if (prev_param == 'e') sort_by_en_chars = 0;
                    break;

                case 'l':
                    if (prev_param == 'n') nologo = 1;
                    break;

                case 't':
                    measure_time = 1;

                //TODO: default:

                }
                prev_param = param;
            }
        }
        if (!nologo)
            printf(LOGO);
    }

    if (measure_time) {
        Stopwatch(SW_START);
    }

    FILE* Source = NULL;
    FILE*    out = NULL;
    if (OpenFile(INPUT, "rb", &Source) != 0
     || OpenFile(OUTPUT, "w", &out)    != 0) {
        return 0;
    }

    int FileSize = GetFileSize(INPUT);
    unsigned char* data = (unsigned char*) calloc(FileSize + 1, sizeof(data[0]));

    int StringsCount = GetDataAndCountStrings(Source, data, FileSize);
    line* pointers = (line*) calloc(StringsCount, sizeof(pointers[0]));
    GetNewStringsPointers(data, pointers);

    //--------------------------------------------------------------------------------------------------------------------------------
        //Sorting and printing

    //1. Directly sorted
    //MergeSort(pointers, StringsCount, 1);
    qsort((void*) pointers, (size_t) StringsCount, sizeof(line), DirCompareTo);
    fPrintLines(out, pointers, StringsCount);

    //2. Reverse sorted
    MergeSort(pointers, StringsCount, -1);
    //qsort((void*)pointers, (size_t)StringsCount, sizeof(line), RevCompareTo);
    fPrintLines(out, pointers, StringsCount);

    //3. Original ver
    fPrintStrings(out, data);


    fclose(out);
    free(pointers);
    free(data);


    if (measure_time) printf("%f", Stopwatch(SW_END));
    return 0;
}

int OpenFile(const char* filename, const char* mode, FILE** stream) {

    int errcode = 0;
    if ((errcode = fopen_s(stream, filename, mode)) != 0) {
        printf("Unexpected error while opening %s\n", filename);
        return errcode;
    }
    return 0;

}

int GetFileSize(const char* filename) {
    assert(filename);

    struct stat fileProperties = {};
    int errcode = stat(filename, &fileProperties);

    if (errcode != 0) return -1;
    return fileProperties.st_size;

}

void fPrintLines(FILE* out, struct line* array, int size) {
    assert (out != NULL);
    assert (array != NULL);

    for (int i = 0; i < size; i++) {
        fprintf(out, "%s\n", array[i].start);
    }

    for (int i = 0; i < 50; i++) {
        fprintf(out, "*\n");
    }

}

void fPrintStrings(FILE* out, unsigned char* array) {
    assert(out != NULL);
    assert(array != NULL);

    if (*(array + 1) != (unsigned char)EOF) fprintf(out, "%s\n", array);
    for (int i = 0; *(array + i + 1) != (unsigned char)EOF; i++) {
        if (*(array + i - 1) == '\0') {
            fprintf(out, "%s\n", array + i);
        }
    }

}

void GetNewStringsPointers(unsigned char* data, struct line* pointers) {

    assert(data != NULL);

    (*pointers).start = data;
    pointers++;
    int i = 0;
    for (; *(data + i + 1) != (unsigned char)EOF; i++) {
        if (*(data + i) == '\0') {
            (*pointers).start = data + i + 1;
            (*(pointers - 1)).end = data + i - 1;
            pointers++;
        }
    }
    if (*(data + i) == '\0') {
        (*(pointers - 1)).end = data + i - 1;
        pointers++;
    }


}


int GetDataAndCountStrings(FILE* Source, unsigned char* data, int FileSize) {

    assert(Source != NULL);
    assert(data != NULL);

    fread(data, sizeof(unsigned char), FileSize, Source);

    int counter = 0;

    unsigned char* writer = data;
    unsigned char current_wrote = '\0';

    for (int i = 0; *(data+i); i++) {
        if (*(data+i) == '\r') continue;

        if (*(data + i) == '\n') {
            if (current_wrote == '\0') continue;
            
            *writer = '\0';
            writer++;
            current_wrote = '\0';
            counter++;
        }
        else {
            *writer = *(data + i);
            writer++;
            current_wrote = *(data + i);
        }

    }
    if (current_wrote != '\0')
    {
        *writer = '\0';
        writer++;
        counter++;
    }
    *(writer++) = EOF;
    realloc(data, writer - data);
    //TODO: reallocate data (сделано)

    return counter;

}


int GetCharCode(unsigned char* ch, int dir) {
    assert(dir == 1 || dir == -1);

    if ((*ch) == '\0') {
        return 0;
    }

    if (dir == -1) {  //На вход получен младший байт символа, нужно перейти к старшему
        int bytes = 1;
        while (((*ch) & 0b11000000) == 0b10000000) { // *ch имеет вид 0b10xxxxxx
            ch--;
            bytes++;
        }
    }

    if (((*ch) & 0b10000000) == 0) { //Символ занимает 1 байт
        if (sort_by_en_chars && ((*ch >= 'A' && *ch <= 'Z') || (*ch >= 'a' && *ch <= 'z')) 
         || sort_by_digits   &&  (*ch >= '0' && *ch <= '9')) {
            return *ch;
        }

        else {
            return -1;
        }
    }

    if (((*ch) & 0b11100000) == 0b11000000) { //Символ занимает 2 байт
        if (!sort_by_ru_chars) return -2;
        int fullcode = (((int)*ch) << 8) + (int)*(ch + 1);

        if (fullcode >= 0xD090 && fullcode <= 0xD095) { //А...Е
            return fullcode;
        }

        if (fullcode == 0xD001) { //Ё
            return 0xD096;
        }

        if (fullcode >= 0xD096 && fullcode <= 0xD0B5) { //Ж...е
            return fullcode + 1;
        }

        if (fullcode == 0xD191) { //ё
            return 0xD0B7;
        }

        if (fullcode >= 0xD0B6 && fullcode <= 0xD0BF || fullcode >= 0xD180 && fullcode <= 0xD18F) { //ж...я
            return fullcode + 2;
        }

        return -2;
    }

    if (((*ch) & 0b11110000) == 0b11100000) { //Символ занимает 3 байт
        return -3;
    }

    return -4; // Символ занимает 4 байт

}


int dct_strcmp(struct line str1, struct line str2, int dir) {

    assert(str1.start != NULL);
    assert(str2.start != NULL);
    assert(str2.end != NULL);
    assert(str2.end != NULL);
    assert(dir == 1 || dir == -1);

    unsigned char *i1, *i2;
    if (dir == -1) {
        i1 = str1.end;
        i2 = str2.end;
    }
    else {
        i1 = str1.start;
        i2 = str2.start;
    }

    int code1 = 0, code2 = 0;
    while (1) {

        //Пропуск невалидных символов str1
        while ((code1 = GetCharCode(i1, dir)) < 0) {
            i1 -= code1 * dir;
        }

        //Пропуск невалидных символов str2
        while ((code2 = GetCharCode(i2, dir)) < 0) {
            i2 -= code2 * dir;
        }

        if (code1 != code2) {
            if (code1 == 0) return  1;
            if (code2 == 0) return -1;
            return code1 - code2;
        }

        if (code1 == '\0') {
            return 0;
        }

        while (code1 > 0) { //Сдвинуть указатели столько раз, сколько байт занимают символы по их значениям сейчас
            i1 += dir;
            i2 += dir;
            code1 >>= 8;
        }
    }

}


int DirCompareTo(const void* val1, const void* val2) {
    return dct_strcmp(*(line*)val1, *(line*)val2 , 1);
}


int RevCompareTo(const void* val1, const void* val2) {
    return dct_strcmp(*(line*)val1, *(line*)val2, -1);
}


/*!
    This function enplaces elements from sorted char** \c from[diap1:enddiap1] and sorted char** \c from[diap2:enddiap2]
        to char** \c to[diap1:enddiap2].
    \warning These conditions must be met:

    \warning \c diap1 <= \c enddiap1 == \c diap2 < \c enddiap2

    \warning for any integer \a i \c diap1 <= \a i < \c enddiap2 \c to[\a i ] must be valid address for writing
                                      and \c from[\a i ] must be valid address for reading
    \param [IN]    diap1 - the  left edge of  left range
    \param [IN]    diap2 - the  left edge of right range
    \param [IN] enddiap1 - the right edge of  left range
    \param [IN] enddiap2 - the right edge of right range

*/
void Merging(int diap1, int diap2, int enddiap1, int enddiap2, struct line* from, struct line* to, int dir) {
    assert(diap1 <= enddiap1);
    assert(diap2 == enddiap1);
    assert(diap2 <= enddiap2);

    to = to + diap1;
    while (1) {
        if (diap1 < enddiap1) {
            if (diap2 < enddiap2) {
                if (dct_strcmp(from[diap1], from[diap2], dir) > 0) {
                    *to = from[diap2];
                    diap2++;
                }
                else {
                    *to = from[diap1];
                    diap1++;
                }
                to++;
            }
            else { // второй диапазон кончился
                while (diap1 < enddiap1) {
                    *to = from[diap1];
                    diap1++;
                    to++;
                }
            }
        }
        else {
            while (diap2 < enddiap2) {
                *to = from[diap2];
                diap2++;
                to++;
            }
            return;
        }
    }
}


/*!
    Sorting char** \c array from element \a 0 to \a size-1 with comparator \b int \b dir_strcmp(line,line)
*/
int MergeSort(struct line* array, int size, int dir) {

    line* array2 = (line*) calloc(size, sizeof(array2[0]));
    //for (int i = 0; i < size; i++) { *(array2 + i) = NULL; }

    int diap1 = 0, diap2 = 0, enddiap1 = 0, enddiap2 = 0;
    int oddity = 1;

    for (int tpstep = 1; tpstep < size; tpstep *= 2, oddity = (!oddity)) {

        int diap1 = 0, diap2 = 0, enddiap1 = 0, enddiap2 = 0;

        while (enddiap2 != size) {
            diap1 = enddiap2;
            enddiap1 = (diap1 + tpstep < size) ? diap1 + tpstep : size;
            diap2 = enddiap1;
            enddiap2 = (diap2 + tpstep < size) ? diap2 + tpstep : size;

            if (oddity) {
                Merging(diap1, diap2, enddiap1, enddiap2, array, array2, dir);
            } 
            
            else {
                Merging(diap1, diap2, enddiap1, enddiap2, array2, array, dir);
            }
        }
    }

    if (!oddity) Merging(0, size, size, size, array2, array, dir);
    free(array2);
    return 0;

}
