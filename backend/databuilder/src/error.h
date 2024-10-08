#ifndef _ERROR_H
#define _ERROR_H

#include <bits/stdc++.h>


// DO NOT USE THIS IF STARTED TO WRITE DATA. FAIL ONLY IN READING AND PROCESSING
void critical(std::string err)
{
    std::cerr << "THERE WAS A CRITICAL ERROR! STOPING EXECUTION\n"
              << err << "\n";
    exit(1);
}

void warning(std::string err)
{
    std::cerr << "WARNING: " << err << "\n";
}
#endif
