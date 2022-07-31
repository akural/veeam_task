#include "Veeam.h"

using namespace std;

shared_ptr<char[]> g_buffer;

int main(int argc, char* argv[])
{
    int numberOfThread = 1;
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
    vector<boost::thread> thrRead;
    vector<boost::thread> thrWrite;
    for (int i = 0; i < numberOfThread; i++) 
    {
        thrRead.push_back({ ReadBuffer, bufferSize, fileIn, bufferSize });
        thrWrite.push_back({ WriteHash, fileOut, bufferSize, sizeB });
        thrRead[i].join();
        thrWrite[i].join();
    }

    return 0;
}