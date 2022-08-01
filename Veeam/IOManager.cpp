#include "IOManager.h"

using namespace std;

boost::atomic_bool closeFile = false;   // When input file is closed
boost::mutex m2;
std::array<boost::mutex, 2> bufMut;     // These mutexes help to write and read in buffer

// Global buffer where input data stores
//One thread can use the first half of buffer, another - second
extern shared_ptr<char[]> g_buffer;

// Track size of the last block of the input file for first and second part of file
std::array < boost::atomic_int, 2> dataReady{0, 0};

// Helps to start reading and writing on time
boost::condition_variable condWrite;
boost::condition_variable condRead;

std::streamsize GetFileSize(ifstream& a_file)
{
    if (a_file.is_open())
    {
        a_file.ignore(std::numeric_limits<std::streamsize>::max());
        std::streamsize length = a_file.gcount();
        a_file.clear();   //  Since ignore will have set eof.
        a_file.seekg(0, std::ios_base::beg);
        return length;
    }
    return { 0 };
}

InputManager::InputManager(const std::string& a_fileName, char* a_buffer, int a_sizeBlock) : sizeBlock(a_sizeBlock), m_buffer(a_buffer)
{
    inputFile.exceptions(ifstream::badbit);
    try
    {
        inputFile.open(a_fileName, std::ifstream::binary);
    }
    catch (const ifstream::failure& ) {
        cout << "Error with opening file " << a_fileName << endl;
    }
}

void InputManager::GetBlockInFile(int a_sizeBuffer)
{
    long long offsetFile = 0;
    long long sizeFile = GetFileSize(inputFile);
    if (!sizeFile)
    {
        cout << "Empty input file" << endl;
    }

    while (!closeFile)
    {
        boost::unique_lock<boost::mutex> lock2(m2);
        // While data isn't be read
        while (all_of(dataReady.begin(), dataReady.end(), [](const auto& item) { return item != 0; }))
            condWrite.wait(lock2);

        for (int i = 0; i < dataReady.size(); i++)
        {
            boost::lock_guard<boost::mutex> lock(bufMut[i]);
            if (dataReady[i])
                continue;
            /*
             * It can be two situations: input file can be divided in equal size and takes whole
             * buffer or not. So, one must take this piece, do pudding with null.
             * First we use first half of the buffer, then second
             */
            long long littlePiece = sizeFile - offsetFile;
            dataReady[i] = littlePiece;
            try
            {
                if (a_sizeBuffer / 2 - 1 <= littlePiece)
                    inputFile.read(m_buffer + i * a_sizeBuffer / 2, a_sizeBuffer / 2);
                else {
                    inputFile.read(m_buffer + i * a_sizeBuffer / 2, littlePiece);
                    memset(m_buffer + i * a_sizeBuffer / 2 + littlePiece, 0,
                           (size_t)(a_sizeBuffer / 2 - (littlePiece)));
                }
            }
            catch (const ifstream::failure& )
            {
                cout << "Error with reading from the file" << endl;
                return;
            }

            offsetFile += a_sizeBuffer / 2;
            condRead.notify_one();
            // This condition says that the whole input file was read.
            if (offsetFile >= sizeFile)
            {
                closeFile = true;
                break;
            }
        }
    }
}

InputManager::~InputManager()
{
    inputFile.close();
}

OutputManager::OutputManager(const string& name)
{
    m_file.exceptions(ofstream::badbit);
    try {
        m_file.open(name, std::ofstream::binary);
    }
    catch (const ofstream::failure& ) {
        cout << "Error with opening file" << endl;
    }

}

OutputManager::~OutputManager()
{
    m_file.close();
}

void OutputManager::WriteInto(char* a_buffer, int a_sizeBuffer, int a_sizeBlock)
{
    while (1)
    {
        int tmp = 0;

        // Because buffer is divided in two parts, one of them can be used for writing
        // while reading is happening with another. DataReady defines which part of buffer is ready
        // and Value of dataready[i] defines size of last useful block
        boost::unique_lock<boost::mutex> lock(bufMut[tmp]);
        while (!dataReady[0] && !dataReady[1])
            condRead.wait(lock);

        tmp = (dataReady[0]) ? 0 : 1;

        printHashIntoFile(a_buffer + tmp * a_sizeBuffer/2, a_sizeBuffer/2, a_sizeBlock, dataReady[tmp]);

        // For half tmp of the buffer reading process can be started
        condWrite.notify_one();
        dataReady[tmp] = 0;

        // if every block is empty reading stopped.
        if (closeFile && !dataReady[0] && !dataReady[1])
            return;
    }
}

void OutputManager::printHashIntoFile(char* a_buffer, int a_sizeBuffer, int a_sizeBlock, boost::atomic_int& a_dataReady)
{
    int start = 0;
    int end = a_sizeBlock;
    string out;
    for (start = 0; start < a_sizeBuffer; start += a_sizeBlock, end += a_sizeBlock)
    {
        std::string txt(a_buffer + start, a_sizeBlock);
        md5(txt, out);
        try
        {
            m_file << out << "\n";
        }
        catch (const ofstream::failure& )
        {
            cout << "Error with writing into the file" << endl;
        }
        // After value of dataReady buffer is useless
        if (end >= a_dataReady)
            break;
    }
}

// To start writing into file "name"
void WriteHash(const string& a_name, int a_sizeBuffer, int a_sizeBlock)
{
    OutputManager hashW(a_name);
    if (g_buffer == nullptr)
        return;
    hashW.WriteInto(g_buffer.get(), a_sizeBuffer, a_sizeBlock);
}

// To start reading from input file "name"
void ReadBuffer(int a_sizeBlock, const string& a_name, int a_sizeBuffer)
{
    InputManager fileM(a_name, reinterpret_cast<char*>(g_buffer.get()), a_sizeBlock);
    fileM.GetBlockInFile(a_sizeBuffer);
}