#pragma once

#include <Poco/Process.h>
#include <Poco/Pipe.h>
#include <Poco/PipeStream.h>
#include <Poco/StreamCopier.h>

#include <sstream>

using namespace Poco;
using Poco::Process;
using Poco::Pipe;
using Poco::PipeInputStream;
using Poco::PipeOutputStream;
using Poco::ProcessHandle;
using Poco::StreamCopier;

class ThreadShellExec : public ofThread {
public:
    ThreadShellExec(): done(false)
    {
    }

    void start()
    {
        startThread();
    }

    virtual int shellExec() {
    }

    void threadedFunction()
    {
        if(lock())
        {
            shellExec();

            unlock();
            stopThread();
        }
        else
        {
            // If we reach this else statement, it means that we could not
            // lock our mutex, and so we do not need to call unlock().
            // Calling unlock without locking will lead to problems.
            ofLogWarning("threadedFunction()") << "Unable to lock mutex.";
        }
    }

    int isDone()
    {
        ofScopedLock lock(mutex);
        return done;
    }

protected:
    // This is a simple variable that we aim to always access from both the
    // main thread AND this threaded object.  Therefore, we need to protect it
    // with the mutex.  In the case of simple numerical variables, some
    // garuntee thread safety for small integral types, but for the sake of
    // illustration, we use an int.  This int could represent ANY complex data
    // type that needs to be protected.
    bool done;
};

class ThreadBookshelf : public ThreadShellExec
{
public:
    ThreadBookshelf()
    {
    }

    virtual int shellExec() {
        // install the bookshelf
        std::string cmd("k2hcopybookshelf");
        std::vector<std::string> args;
        Poco::Pipe outPipe;
        ProcessHandle ph = Process::launch(cmd, args, 0, &outPipe, 0);
        Poco::PipeInputStream istr(outPipe);
        //ph.wait();

        stringstream out;
        Poco::StreamCopier::copyStream(istr, out);
        ofLogVerbose() << "command [installbookshelf]: " << out.str();

        int rc = ph.wait();
        done = true;

        return rc;
    }
};


class ThreadPrint : public ThreadShellExec
{
public:
    ThreadPrint()
    {
    }

    virtual int shellExec() {
        // install the bookshelf
        std::string cmd("k2h");
        std::vector<std::string> args;
        args.push_back("-pdf /tmp/label.pdf");
        Poco::Pipe outPipe;
        ProcessHandle ph = Process::launch(cmd, args, 0, &outPipe, 0);
        Poco::PipeInputStream istr(outPipe);
        //ph.wait();

        stringstream out;
        Poco::StreamCopier::copyStream(istr, out);
        ofLogVerbose() << "command [printlabel]: " << out.str();

        int rc = ph.wait();
        done = true;

        return rc;
    }
};
