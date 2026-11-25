
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
using namespace std;

queue<int> buffer;
mutex mtx;
condition_variable cv;
atomic<int> producedCount(0);
atomic<bool> finished(false);

void producer(int count)
{
    for (int i = 1; i <= count; i++)
    {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] {
            return buffer.empty();
        });

        buffer.push(i);
        producedCount++;
        cout << "Produced: " << i << endl;

        lock.unlock();
        cv.notify_one();
    }

    {
        lock_guard<mutex> lock(mtx);
        finished = true;
    }
    cv.notify_all();
}

void consumer()
{
    while (true)
    {
        unique_lock<mutex> lock(mtx);

        cv.wait(lock, [] {
            return !buffer.empty() || finished;
        });

        if (buffer.empty() && finished)
            break;

        int item = buffer.front();
        buffer.pop();

        cout << "Consumed: " << item << endl;

        lock.unlock();
        cv.notify_one();
    }
}

void runTest(int testNo, int count)
{
    cout << "===== Test Case " << testNo << " =====" << endl;
    cout << "Producer-Consumer System Started" << endl;

    producedCount = 0;
    finished = false;

    thread producerThread(producer, count);
    thread consumerThread(consumer);

    producerThread.join();
    consumerThread.join();

    cout << "Total Items Produced: " << producedCount << endl;
    cout << "Producer-Consumer System Completed" << endl;
    cout << endl;
}

int main()
{
    runTest(1, 5);
    runTest(2, 7);

    cout << "Program completed successfully." << endl;
    return 0;
}
