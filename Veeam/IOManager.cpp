#include "IOManager.h"

using namespace std;

boost::atomic_bool closeFile = false;
boost::mutex m2;
std::array<boost::mutex, 2> bufMut;
extern shared_ptr<char[]> g_buffer;

// Track size of the last block of the input file
std::array < boost::atomic_int, 2> dataReady{0, 0};
boost::condition_variable condWrite;
boost::condition_variable condRead;

//ToDo
/*
 * .ignore
 *  everys codestyle
 *   inputFile.exceptions(ifstream::badbit);
 */
std::streamsize GetFileSize(ifstream& file)
{
    if (file.is_open())
    {
        file.ignore(std::numeric_limits<std::streamsize>::max());
        std::streamsize length = file.gcount();
        file.clear();   //  Since ignore will have set eof.
        file.seekg(0, std::ios_base::beg);
        return length;
    }
    return { 0 };
}

InputManager::InputManager(const std::string& fileName, char* buffer, int sizeBlock) : sizeBlock(sizeBlock), m_buffer(buffer)
{
    inputFile.exceptions(ifstream::badbit);
    try {
        inputFile.open(fileName, std::ifstream::binary);
    }
    catch (const ifstream::failure& ) {
        cout << "Error with opening file " << fileName << endl;
    }
}

void InputManager::GetBlockInFile(int sizeBuffer)
{
    long long offsetFile = 0;
    long long sizeFile = GetFileSize(inputFile);
    if (!sizeFile)
    {
        cout << "Empty input file" << endl;
    }
    try {
        while (!closeFile)
        {
            boost::unique_lock<boost::mutex> lock2(m2);
            while (all_of(dataReady.begin(), dataReady.end(), [](const auto& item) { return item != 0; }))
                condWrite.wait(lock2);

            for (int i = 0; i < dataReady.size(); i++)
            {
                boost::lock_guard<boost::mutex> lock(bufMut[i]);
                if (dataReady[i])
                    continue;

                long long littlePiece = sizeFile - offsetFile;
                dataReady[i] = littlePiece;
//комменты сюда
                if (sizeBuffer / 2 - 1 <= littlePiece)
                    inputFile.read(m_buffer + i * sizeBuffer / 2, sizeBuffer / 2);
                else
                {
                    inputFile.read(m_buffer + i * sizeBuffer / 2, littlePiece);
                    memset(m_buffer + i * sizeBuffer / 2 + littlePiece, 0, (size_t)(sizeBuffer / 2 - (littlePiece)));
                }
                offsetFile += sizeBuffer / 2;
                condRead.notify_one();
                if (offsetFile >= sizeFile)
                {
                    closeFile = true;
                    break;
                }
            }
        }
    }
    // эксит сюда
    catch (const ifstream::failure& )
    {
        cout << "Error with reading from the file" << endl;
    }
}

InputManager::~InputManager()
{
    inputFile.close();
}
// нахера
OutputManager::OutputManager(const string& name)
{
    file.exceptions(ofstream::badbit);
    try {
        file.open(name, std::ofstream::binary);
    }
    catch (const ofstream::failure& ) {
        cout << "Error with opening file" << endl;
    }

}

OutputManager::~OutputManager()
{
    file.close();
}

void OutputManager::WriteInto(char* buffer, int sizeBuffer, int sizeBlock)
{
    while (1)
    {
        int tmp = 0;
        // закоментить
        boost::unique_lock<boost::mutex> lock(bufMut[tmp]);
        while (!dataReady[0] && !dataReady[1])
            condRead.wait(lock);

        tmp = (dataReady[0]) ? 0 : 1;

        printHashIntoFile(buffer + tmp * sizeBuffer/2, sizeBuffer/2, sizeBlock, dataReady[tmp]);

        condWrite.notify_one();
        dataReady[tmp] = 0;

        if (!dataReady[0] && !dataReady[1])
            return;
    }
}

void OutputManager::printHashIntoFile(char* buffer, int sizeBuffer, int sizeBlock, boost::atomic_int& dataReady)
{
    // menshe try
    try {
        int start = 0;
        int end = sizeBlock;
        string out;
        for (start = 0; start < sizeBuffer; start += sizeBlock, end += sizeBlock)
        {
            std::string txt(buffer + start, sizeBlock);
            md5(txt, out);
            file << out << "\n";
            if (end >= dataReady)
                break;
        }
    }
    catch (const ofstream::failure& ) {
        cout << "Error with writing into the file" << endl;
    }
}

// To start writing into file "name"
void WriteHash(const string& name, int sizeBuffer, int sizeBlock)
{
    OutputManager hashW(name);
    if (g_buffer == nullptr)
        return;
    hashW.WriteInto(g_buffer.get(), sizeBuffer, sizeBlock);

}

// To start reading from input file "name"
void ReadBuffer(int sizeBlock, const string& name, int sizeBuffer)
{
    InputManager fileM(name, reinterpret_cast<char*>(g_buffer.get()), sizeBlock);
    fileM.GetBlockInFile(sizeBuffer);
}