/**
 * Reader-Writer問題をセマフォ(状態変数)を使って解決せよ
 * 以下の特徴をキープすること
 *    1. Readerは複数同時にいても問題ない
 *    2. Readerがいる場合、Writerは書けない(WriteしているときはReaderができない)
 *    3. Writerは複数同時にいてはいけない
 */

#include <iostream>
#include <vector>
#include <pthread.h>
#include <condition_variable>

#include <unistd.h>

namespace {
    const int WORD_SIZE = 10;
    const int RANDOM_WORD_SIZE = 20;
    const int W_TH_NUM = 3;
    const int R_TH_NUM = 10;
}  // namespace

class RWLock
{
public:
    RWLock()
    : _readingReaders(0)
    , _writingWriters(0)
    , _waitingWriters(0)
    , _preferReader(true)
    {
    }

    void readOpen()
    {
        std::unique_lock<std::mutex> rlock(_mtx);

        // Writerが0 かつ (読み手優先 か 待ちWriterが0) になるまで待つ
        _not_writing.wait(rlock, [this] { return _writingWriters <= 0 && (_preferReader || _waitingWriters <= 0); });

        _readingReaders++;
        printf("Read open. Reader:%d\n", _readingReaders);
    }

    void readClose()
    {
        std::lock_guard<std::mutex> rlock(_mtx);
        _readingReaders--;
        _preferReader = false;  // 書き手を優先させる
        // 待ちスレッドに知らせる
        _not_writing.notify_all();
        printf("Read close. Reader:%d\n", _readingReaders);
    }

    void writeOpen()
    {
        std::unique_lock<std::mutex> wlock(_mtx);

        _waitingWriters++;

        // Readerが0 かつ Writerが0 になるまで待つ
        _not_writing.wait(wlock, [this]{ return _readingReaders <= 0 && _writingWriters <= 0; });

        _waitingWriters--;

        _writingWriters++;
        printf("Write open. WaitingWriter:%d\n", _waitingWriters);
    }

    void writeClose()
    {
        std::lock_guard<std::mutex> wlock(_mtx);
        _writingWriters--;
        _preferReader = true;  // 読み手を優先させる
        // 待ちスレッドに知らせる
        _not_writing.notify_all();
        printf("Write close.\n");
    }

private:
    int _readingReaders;
    int _writingWriters;
    int _waitingWriters;
    bool _preferReader;
    std::mutex _mtx;
    std::condition_variable _not_writing;
};

void randomSleep()
{
    int time = rand() % 2;
    sleep(time);
}

class Data
{
public:
    char* read(char* word)
    {
        _rwLock.readOpen();
        strncpy(word, _buffer, WORD_SIZE);
        word[WORD_SIZE] = '\n';
        randomSleep();
        _rwLock.readClose();

        return word;
    }

    void write(char* word)
    {
        _rwLock.writeOpen();

        int offs = (rand() % RANDOM_WORD_SIZE);
        char* buf2 = word + sizeof(char) * offs;
        char c = buf2[0];
        for (int i = 0; i < WORD_SIZE - 1; i++) {
            _buffer[i] = c;
            randomSleep();
        }
        _rwLock.writeClose();
    }

private:
    char _buffer[WORD_SIZE];
    RWLock _rwLock;
};

// 一定タイミングでファイルを書きに行く人
void* writer(void* p)
{
    randomSleep();

    Data* data = (Data*)p;
    char word[] = "ABCDEFGHIJKLMN*+-#,.";
    data->write(word);

    pthread_exit(NULL);
}

// 一定タイミングでファイルを読みに行く人
void* reader(void* p)
{
    randomSleep();

    Data* data = (Data*)p;
    char word[WORD_SIZE];
    char* ret;
    ret = data->read(word);
    printf("read: %s\n", ret);

    pthread_exit(NULL);
}

int main()
{
    Data* data = new Data();

    /* スレッドを起動する */
    pthread_t writeThreads[W_TH_NUM];
    for (int i = 0; i < W_TH_NUM; i++) {
        pthread_create(&writeThreads[i], NULL, writer, (void*)data);
    }

    /* スレッドを起動する */
    pthread_t readThreads[R_TH_NUM];
    for (int i = 0; i < R_TH_NUM; i++) {
        pthread_create(&readThreads[i], NULL, reader, (void*)data);
    }

    /* スレッドの終了を待つ */
    for (auto writeThread : writeThreads) {
        pthread_join(writeThread, NULL);
    }
    for (auto readThread : readThreads) {
        pthread_join(readThread, NULL);
    }

    return 0;
}
