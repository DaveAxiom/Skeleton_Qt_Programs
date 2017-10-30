#include <QCoreApplication>
#include <QObject>
#include <QThread>
#include <QDebug>

class LogicProcess : public QObject
{
    Q_OBJECT

public:
    LogicProcess() {}

    void init(QThread* thread)
    {
        m_thread = thread;
        connect(thread, SIGNAL(started()), this, SLOT(run()));
        connect(thread, SIGNAL(finished()), this, SLOT(PostThread()));
    }

public slots:
    void run()
    {
        qDebug("LogicProcess::run() reached.");

        //Computation executes here.

        m_thread->exit();
    }

    void PostThread()
    {
        qDebug("LogicProcess::PostThread() reached.");
    }
private:
    QThread* m_thread;
};


class ThreadContainer : public QObject
{
    Q_OBJECT

public:
    ThreadContainer()
    {
        m_thread = NULL;
        m_logic = NULL;
    }

    ~ThreadContainer()    {}

private:
    QThread* m_thread;
    LogicProcess* m_logic;


public slots:
    void PostThread()
    {
        qDebug("ThreadContainer::PostThread() reached.");
    }

    void ThreadStart()
    {
        qDebug("ThreadContainer::ThreadStart() reached.");
    }

public:
    void ComputeThreadSpawner()
    {
        if (m_thread == NULL)
            m_thread = new QThread();
        else
        {
            qDebug("Wait method.");
            m_thread->wait();
            qDebug("Wait method done.");
        }

        if (m_logic == NULL)
        {
            m_logic = new LogicProcess();
            connect(m_thread, SIGNAL(started()), this, SLOT(ThreadStart()));
            connect(m_thread, SIGNAL(finished()), this, SLOT(PostThread()));
            m_logic->init(m_thread);
            m_logic->moveToThread(m_thread);
        }

        if (!m_thread->isRunning() || m_thread->isFinished())
            m_thread->start();
    }
};


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ThreadContainer* b = new ThreadContainer();
    b->ComputeThreadSpawner();
    b->ComputeThreadSpawner();

    return a.exec();
}
#include "main.moc"
