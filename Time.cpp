#include <time.h>

/*!
    \param [IN] par = 0 to start, 1 to get time in seconds gone since last start
    \return 0 if successfully started
    \return positive or above zero value if successfully calculated time gone
    \return negative value otherwise
    \note if you unexpectedly got negative value, check if you passed correct parameter to the function and passed 0 before passing 1
*/
double Stopwatch(int par) {
    static clock_t start;
    if (par == 0) {
        start = clock();
        return 0;
    }
    else if (par == 1) {
        return ((double)clock() - (double)start) / CLK_TCK;
    }
    return -1;
}