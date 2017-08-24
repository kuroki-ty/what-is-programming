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

class RWLock
{
public:
    void writeOpen()
    {
        printf("Write open.\n");
        _writerNum++;
    }

    void readOpen()
    {
        printf("Read open.\n");
        _readerNum++;
    }

    void writeClose()
    {
        printf("Write close.\n");
        _writerNum--;
    }

    void readClose()
    {
        printf("Read close.\n");
        _readerNum--;
    }

    void write(const char* word)
    {
        // ファイルに書き込む とりあえず文字連結
        strcat(_file, word);
    }

    char* read()
    {
        return _file;
    }

    std::unique_lock<std::mutex> getWLock()
    {
        return std::unique_lock<std::mutex>(_wmtx);
    }

    int getWriterNum() { return _writerNum; }
    int getReaderNum() { return _readerNum; }

private:
    // ファイル とりあえず変数にしとく
    char _file[256];

    int _writerNum = 0;
    int _readerNum = 0;
    std::mutex _wmtx;
};

RWLock rwLock;
std::condition_variable reading;
std::condition_variable writing;

void randomSleep()
{
    int time = rand() % 3;
    sleep(time);
}

// 一定タイミングでファイルを書きに行く人
void* writer(void *)
{
    std::unique_lock<std::mutex> wlock = rwLock.getWLock();
    if (rwLock.getReaderNum() > 0) {
        reading.wait(wlock, [] { return rwLock.getReaderNum() <= 0; });
    }
    rwLock.writeOpen();
    randomSleep();
    rwLock.write("x ");
    rwLock.writeClose();

    writing.notify_all();

    pthread_exit(NULL);
    return nullptr;
}

// 一定タイミングでファイルを読みに行く人
void* reader(void *)
{
    if (rwLock.getWriterNum() > 0) {
        std::mutex rmtx;
        std::unique_lock<std::mutex> rlock(rmtx);
        writing.wait(rlock);
    }

    rwLock.readOpen();
    randomSleep();
    char* word = rwLock.read();
    printf("R:%s\n", word);
    rwLock.readClose();

    reading.notify_one();

    pthread_exit(NULL);
    return nullptr;
}

int main()
{
    const int W_TH_NUM = 3;
    const int R_TH_NUM = 3;

    /* スレッドを起動する */
    pthread_t writeThreads[W_TH_NUM];
    for (int i = 0; i < W_TH_NUM; i++) {
        pthread_create(&writeThreads[i], NULL, writer, NULL);
    }

    /* スレッドを起動する */
    pthread_t readThreads[R_TH_NUM];
    for (int i = 0; i < R_TH_NUM; i++) {
        pthread_create(&readThreads[i], NULL, reader, NULL);
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
