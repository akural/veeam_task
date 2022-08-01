#include "Veeam.h"

using namespace std;

// Global buffer for store data for input part. Practically our program uses first half of the buffer,
// Then second. In other words one thread uses first part while another uses another part.
shared_ptr<char[]> g_buffer;

int main(int argc, char* argv[])
{
    string fileIn, fileOut;
    int sizeB = MB;
    g_buffer.reset(new char[bufferSize]); // 1 GB

    if (argc > 0) {
        try
        {
            std::tie(fileIn, fileOut, sizeB) = CheckCMD(argc, argv);
        }
        catch (WrongBlock& ex)
        {
            cout << ex.what() << endl;
            return -2;
        }
        catch (WrongArgs& ex)
        {
            cout << ex.what() << endl;
            return -2;
        }
    }

    TimeDuration tm;

    boost::thread thrRead{ReadBuffer, bufferSize, fileIn, bufferSize };
    boost::thread thrWrite{ WriteHash, fileOut, bufferSize, sizeB };
    thrRead.join();
    thrWrite.join();

    return 0;
}